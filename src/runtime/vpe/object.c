#include "internal.h"

#define GLOBALS

#include "inter.h"
#include "vpe.h"

#define MAX_COLREGS 4       // Max number of regions to process
#define F_CDIST INT_FIX(16) // Checking distance
#define F_WDIST INT_FIX(8)  // Min walk distance
#define F_SDIST INT_FIX(8)  // Step distance
#define SDIST2 3	    // Step distance (log2)

void AddColReg(struct Region *);

static struct Region *ColReg[MAX_COLREGS];
static int CurColReg, NumColRegs;

//═════════════════════════════════════════════════════════════════════════════
//  Actualiza un objeto
//═════════════════════════════════════════════════════════════════════════════

void DoObjectUpdate(struct Object *po)
{
	struct Point *pp;
	//  struct Object  *pObject; // Para las colisiones
	struct Wall *pWall;
	struct Region *pRegion, *reg;
	struct Move *pSpeed;
	VPEFixed MoveX, MoveY, AddX, AddY, X, Y, AddRH, RegDist;
	VPEFixed c, s, t, t1, t2, tx, ty, x1, y1, x2, y2, x, y, dist, check;
	VPEByte Flag, ColFlag, RotateFlag;
	int i, Steps, hueco;

	//---------------------------------------------------------------------------
	// Destruye un objeto si esta marcado para ser destruido
	//---------------------------------------------------------------------------
	if ((po->Event & EV_DESTROY) != 0) {
		DelPoint(po->pp);
		DelObject(po);
		return;
	}

	pp = po->pp;	   // Object point
	pRegion = po->pRegion; // Object region
	pSpeed = &po->Speed;   // Velocidad

	po->region_number = pRegion->Tag;

	//---------------------------------------------------------------------------
	// Calcula el angulo de giro
	//---------------------------------------------------------------------------
	t1 = FixSin(-po->Angle); //-pSpeed->Y
	t2 = FixCos(-po->Angle); //-pSpeed->Y

	//---------------------------------------------------------------------------
	// Calcula las coordenadas de avance
	//---------------------------------------------------------------------------
	MoveX = FixMul(t2, pSpeed->X);
	MoveY = FixMul(-t1, pSpeed->X);

	//---------------------------------------------------------------------------
	// Deteccion de colisiones
	//---------------------------------------------------------------------------
	AddRH = 0;
	ColFlag = TRUE;
	tx = FixMod(MoveX);
	ty = FixMod(MoveY);
	if (tx >= ty) {
		//-------------------------------------------------------------------------
		// Multiples pasos
		//-------------------------------------------------------------------------
		if (tx > F_SDIST) {
			Steps = (FIX_INT(tx) >> SDIST2) + 1;
			t = FixDiv(INT_FIX(1), INT_FIX(Steps));
			AddX = FixMul(MoveX, t);
			AddY = FixMul(MoveY, t);
		}
		//-------------------------------------------------------------------------
		// Un solo paso
		//-------------------------------------------------------------------------
		else {
			Steps = 1;
			AddX = MoveX;
			AddY = MoveY;
		}
	} else { // MoveY>MoveX
		//-------------------------------------------------------------------------
		// Multiples pasos
		//-------------------------------------------------------------------------
		if (ty > F_SDIST) {
			Steps = (FIX_INT(ty) >> SDIST2) + 1;
			t = FixDiv(INT_FIX(1), INT_FIX(Steps));
			AddX = FixMul(MoveX, t);
			AddY = FixMul(MoveY, t);
		}
		//-------------------------------------------------------------------------
		// Un solo paso
		//-------------------------------------------------------------------------
		else {
			Steps = 1;
			AddX = MoveX;
			AddY = MoveY;
		}
	}

	X = pp->x;
	Y = pp->y; // Coordenadas de inicio

	//---------------------------------------------------------------------------
	// Realiza las colisiones paso a paso
	//---------------------------------------------------------------------------
	for (; Steps > 0; Steps--) {
		pRegion = po->pRegion;
		RegDist = INT_FIX(1000);
		Flag = 0;
		//-------------------------------------------------------------------------
		// Avanza un paso
		//-------------------------------------------------------------------------
		X += AddX;
		Y += AddY;

		//-------------------------------------------------------------------------
		// Inicializa la colision con regiones
		//-------------------------------------------------------------------------
		t1 = pRegion->FloorH;
		t2 = pRegion->CeilH;
		//-------------------------------------------------------------------------
		// Comprueba si colisiona con el suelo
		//-------------------------------------------------------------------------
		if (po->H < t1) {
			po->H = t1;
			ColFlag = FALSE;
		}
		//-------------------------------------------------------------------------
		// Comprueba si colisiona con el techo
		//-------------------------------------------------------------------------
		if (po->H + po->Height >= t2) {
			po->H = t2 - po->Height;
			ColFlag = FALSE;
		}

		//-------------------------------------------------------------------------
		// Comprueba colisiones con objetos y paredes de cada region
		// Comienza por la region del objeto
		//-------------------------------------------------------------------------
		ColReg[0] = pRegion;
		NumColRegs = 1;
		for (CurColReg = 0; CurColReg < NumColRegs; CurColReg++) {
			pRegion = ColReg[CurColReg];
			/*
      //-----------------------------------------------------------------------
      // Colision con objetos
      //-----------------------------------------------------------------------
      pObject=po->Next;
      for(;pObject!=NULL;pObject=pObject->Next) {
	// Check failing conditions
	if (pObject->H>po->H+po->Height)  // po is below
	  continue;
	if (po->H>pObject->H+pObject->Height) // po is above
	  continue;
	// Check if pObject is close to this step
	dist=po->Radius+pObject->Radius;
	if (FixMod(X-pObject->pp->x)>=dist) continue;
	if (FixMod(Y-pObject->pp->y)>=dist) continue;
	// Check structure flags
	// We have to compute exact distance
	tx=pObject->pp->x-X; t1=FIX_INT(tx);
	ty=pObject->pp->y-Y; t2=FIX_INT(ty);
	t=INT_FIX(LongSqrt(t1*t1+t2*t2));
	// Are we too close?
	if (t>INT_FIX(1)&&t<dist) {
	  t1=dist-t;
	  // Total displacement
	  tx=FixMul(t1,FixDiv(tx,t));
	  ty=FixMul(t1,FixDiv(ty,t));
	  // pObject has infinite mass?
	  if (pObject->Mass==0) {
	    // Yep. Move only po.
	    X-=tx; Y-=ty;
	  }
	  // po has infinite mass?
	  else if (po->Mass==0) {
	    // Yep. Move only pObject.
	    MovePoint(pObject->pp,tx,ty);
	  }
	  // Bounce both objects
	  else {
	    // Total mass
	    t=po->Mass+pObject->Mass;
	    tx=FixDiv(tx,t);
	    ty=FixDiv(ty,t);
	    // Our object
	    t1=FixMul(pObject->Mass,tx);
	    t2=FixMul(pObject->Mass,ty);
	    X-=t1; Y-=t2;
	    // The other object
	    t1=FixMul(po->Mass,tx);
	    t2=FixMul(po->Mass,ty);
	    MovePoint(pObject->pp,t1,t2);
	  }
	  ColFlag=FALSE;
	}
      }
*/
			//-----------------------------------------------------------------------
			// Colision con paredes
			//-----------------------------------------------------------------------
			check = F_CDIST + po->Radius; // Distancia de control
			dist =
			    F_WDIST + po->Radius; // Distancia minima para andar
			//-----------------------------------------------------------------------
			// Recorre todas las paredes de la region actual
			//-----------------------------------------------------------------------
			for (i = 0; i < pRegion->NumWallPtrs; i++) {
				pWall = pRegion->WallPtrs[i];
				//---------------------------------------------------------------------
				// Lee las coordenadas
				//---------------------------------------------------------------------
				x1 = pWall->p1->x;
				y1 = pWall->p1->y;
				x2 = pWall->p2->x;
				y2 = pWall->p2->y;
				//---------------------------------------------------------------------
				// Si no esta suficientemente cerca pasa a la
				// siguiente pared
				//---------------------------------------------------------------------
				if (x1 >= x2) {
					if (X <= x2 - check)
						continue;
					if (X > x1 + check)
						continue;
				} else { // x2>x1
					if (X < x1 - check)
						continue;
					if (X >= x2 + check)
						continue;
				}
				if (y1 >= y2) {
					if (Y <= y2 - check)
						continue;
					if (Y > y1 + check)
						continue;
				} else { // y2>y1
					if (Y < y1 - check)
						continue;
					if (Y >= y2 + check)
						continue;
				}
				//---------------------------------------------------------------------
				// Halla el seno y el coseno para la
				// transformacion
				//---------------------------------------------------------------------
				x2 -= x1;
				y2 -= y1;
				t = FIX_INT(FixITan(x2, y2));
				c = FixCos(t);
				s = FixSin(t);
				//---------------------------------------------------------------------
				// Rota X,Y para pasarlo al espacio del muro
				// (espacio afin) X queda a lo largo del muro e
				// Y queda perpendicular
				//---------------------------------------------------------------------
				x = FixMul(c, X - x1) - FixMul(-s, Y - y1);
				y = FixMul(-s, X - x1) + FixMul(c, Y - y1);
				//---------------------------------------------------------------------
				// Si la distancia es mayor que la permitida
				// comprueba el siguiente
				//---------------------------------------------------------------------
				if (FixMod(y) >= dist)
					continue;
				//---------------------------------------------------------------------
				// Si el muro no es solido (puerta,ventana,...)
				//---------------------------------------------------------------------
				RotateFlag = TRUE;
				if ((pWall->Type & W_SOLID) == 0) {
					t1 = pWall->Front->FloorH;
					t2 = pWall->Front->CeilH;
					if (pWall->Back) {
						t1 = FixMax(
						    t1, pWall->Back->FloorH);
						t2 = FixMin(t2,
							    pWall->Back->CeilH);
					}
					hueco = t2 - t1;
					t1 -= po->Step + AddRH;
					//-------------------------------------------------------------------
					// Si entramos por la puerta
					//-------------------------------------------------------------------
					if ((po->H >= t1) &&
					    (po->H + po->Height <= t2) &&
					    (po->Height <= hueco)) {
						//-----------------------------------------------------------------
						// Actualizamos la region
						//-----------------------------------------------------------------
						if (y > 0)
							reg = pWall->Back;
						else
							reg = pWall->Front;
						//-----------------------------------------------------------------
						// Si se sale por los lados
						// comprueba que sea por poco
						//-----------------------------------------------------------------
						if (x < 0 ||
						    x > pWall->Length) {
							t1 = FIX_INT(x);
							if (x > 0)
								t1 -= FIX_INT(
								    pWall
									->Length);
							t2 = FIX_INT(y);
							t = INT_FIX(LongSqrt(
							    t1 * t1 + t2 * t2));
							// Too far from p1
							if (t > po->Radius)
								continue;
							// Very close, change
							// region
							if (t < F_SDIST) {
								reg = FindRegion(
								    X, Y,
								    FixMax(
									pWall
									    ->Front
									    ->FloorH,
									pWall
									    ->Back
									    ->FloorH),
								    0);
								// Should not
								// happen
								if (reg == NULL)
									continue;
								// Change the
								// region?
								if (pRegion !=
								    reg) {
									RegDist =
									    t;
									ClearObjRegion(
									    po);
									SetObjRegion(
									    po,
									    reg);
									if (po->H <
									    reg->FloorH) {
										po->H =
										    reg->FloorH;
									}
									AddRH =
									    po->RH -
									    po->H;
									break;
								}
							}
							// Otherwise, do stairs
							if (po->H >
							    pRegion->FloorH)
								continue;
							if (y > 0)
								reg =
								    pWall
									->Front;
							else
								reg =
								    pWall->Back;
							AddColReg(reg);
							if (reg->FloorH <=
							    pRegion->FloorH)
								continue;
							y = t;
						}
						//-----------------------------------------------------------------
						// Si atraviesa el muro cambia
						// de region si pasa lo
						// suficiente
						//-----------------------------------------------------------------
						else if (y > 0) {
							if (y < RegDist) {
								RegDist = y;
								if (pRegion !=
								    reg) {
									ClearObjRegion(
									    po);
									SetObjRegion(
									    po,
									    reg);
									if (po->H <
									    reg->FloorH) {
										po->H =
										    reg->FloorH;
									}
									AddRH =
									    po->RH -
									    po->H;
									break;
								}
							}
							// Esta suficientemente
							// cerca?
							if (y > po->Radius)
								continue;
							// Down-stairs or
							// flying?
							reg = pWall->Front;

							AddColReg(reg);
							if (reg->FloorH <=
								pRegion
								    ->FloorH ||
							    po->H >
								pRegion->FloorH)
								continue;
						}
						//-----------------------------------------------------------------
						// Se pega al muro sin
						// atravesarlo a menos que este
						// muy cerca
						//-----------------------------------------------------------------
						else if (y < 0) {
							if (-y < RegDist) {
								RegDist = -y;
								// Change from
								// Back to
								// Front?
								if (pRegion !=
								    reg) {
									ClearObjRegion(
									    po);
									SetObjRegion(
									    po,
									    reg);
									// New
									// floor
									// higher?
									if (po->H <
									    reg->FloorH) {
										po->H =
										    reg->FloorH;
									}
									AddRH =
									    po->RH -
									    po->H;
									break;
								}
							}
							// Esta suficientemente
							// cerca?
							if (y < -po->Radius)
								continue;
							// Down-stairs or
							// flying?
							reg = pWall->Back;
							AddColReg(reg);
							if (reg->FloorH <=
								pRegion
								    ->FloorH ||
							    po->H >
								pRegion->FloorH)
								continue;
							y = -y;
						}
						AddRH = FixMax(
						    AddRH,
						    FixDiv(
							FixMul(reg->FloorH,
							       po->Radius - y) +
							    FixMul(
								pRegion->FloorH,
								y),
							po->Radius) -
							po->H);
						// Go on with the wall loop
						continue;
					}
				}

				//---------------------------------------------------------------------
				// Find the distance to move. Object coords in
				// wall space will be updated and then rotated
				// back into world space only if an object is
				// 'inside' the wall.
				//---------------------------------------------------------------------
				if (x < 0) {
					//-------------------------------------------------------------------
					// El objeto esta a la izquierda del
					// muro
					//-------------------------------------------------------------------
					RotateFlag = FALSE;
					t = FIX_INT(FixITan(X - x1, Y - y1));
					tx = FixCos(t);
					ty = FixSin(t);
					if (FixMod(tx) > FixMod(ty))
						t = FixDiv(X - x1, tx);
					else
						t = FixDiv(Y - y1, ty);
					if (t >= dist || t == 0)
						continue;
					// Wall is not moved
					else
						t1 = dist;
					// Move object
					X = x1 + FixMul(t1, tx);
					Y = y1 + FixMul(t1, ty);
				} else if (x > pWall->Length) {
					//-------------------------------------------------------------------
					// El objeto esta a la derecha del muro
					//-------------------------------------------------------------------
					RotateFlag = FALSE;
					t = FIX_INT(FixITan(X - x2, Y - y2));
					tx = FixCos(t);
					ty = FixSin(t);
					if (FixMod(tx) > FixMod(ty))
						t = FixDiv(X - x2, tx);
					else
						t = FixDiv(Y - y2, ty);
					if (t >= dist || t == 0)
						continue;
					// Wall is not moved
					else
						t1 = dist;
					// Move object
					X = x2 + FixMul(t1, tx);
					Y = y2 + FixMul(t1, ty);
				} else {
					//-------------------------------------------------------------------
					// El objeto esta dentro del muro
					//-------------------------------------------------------------------
					po->wall_number = pWall->Tag;

					if (pWall->Front == NULL ||
					    pWall->Back == NULL)
						po->nextregion_number = -1;
					else {
						if (po->region_number ==
						    pWall->Front->Tag)
							po->nextregion_number =
							    pWall->Back->Tag;
						else
							po->nextregion_number =
							    pWall->Front->Tag;
					}

					// Object is in front of the wall.
					if (pRegion == pWall->Front) {
						y = -dist - 1024 * 128;
					}
					// Object is behind the wall.
					else {
						y = dist + 1024 * 128;
					}
				}
				ColFlag = FALSE;
				// Normal coords update.
				if (RotateFlag) {
					// Rotate x,y into world space
					X = FixMul(c, x) - FixMul(s, y) + x1;
					Y = FixMul(s, x) + FixMul(c, y) + y1;
				}
			}
		}
	}

	//---------------------------------------------------------------------------
	// Mueve el objeto
	//---------------------------------------------------------------------------
	po->RH = po->H + AddRH;
	MovePoint(pp, X - pp->x, Y - pp->y);

	pSpeed->X = pSpeed->Y = pSpeed->Z = 0;
}

//═════════════════════════════════════════════════════════════════════════════
//  No se lo que hace
//═════════════════════════════════════════════════════════════════════════════

void AddColReg(struct Region *reg)
{
	int i;

	for (i = 0; i < NumColRegs; i++)
		if (ColReg[i] == reg)
			return;

	if (i < MAX_COLREGS) {
		ColReg[i] = reg;
		NumColRegs++;
	}
}

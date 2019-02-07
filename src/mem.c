#ifdef __DOS__

//───────────────────────────────────────────────────────────────────────────
//───────────────────────────────────────────────────────────────────────────

void *AllocateDOSMem(uint Tam)
{
	union REGS r;

	//───────────────────────────────────────────────────────────────────────────
	// Reserva Tam / 16 Parrafos
	// WARNING: En el documento PMODEW.DOC se especifican los parámetros
	// de la función 100h incorrectamente.
	// Para llamar a una interrupción de modo real, debemos llamar a la
	// interrupción 0x31
	//───────────────────────────────────────────────────────────────────────────

	SetMem(&r, sizeof(r), 0);

	r.x.eax = 0x0100;	  // Función Reservar Bloque Mem. DOS
	r.x.ebx = (Tam + 15) >> 4; // Parrafos a reservar
	int386(0x31, &r, &r);

	// Comprueba error

	if (r.x.cflag)
		return ((uint)_NULL);

	// Retorna puntero a la zona de memoria reservada, pero antes debemos
	// transformar la dirección de modo real a modo protegido, para ello
	// multiplicamos por 16

	return (void *)((r.x.eax & 0xFFFF) << 4);
}

#endif

//═════════════════════════════════════════════════════════════════════════════
// Nucleo de la ejecución de los programas en DIV
//═════════════════════════════════════════════════════════════════════════════
case DIVBC_nop: {
	break;
}
case DIVBC_car: {
	pila[++sp] = mem[ip++];
	break;
}
case DIVBC_asi: {
	pila[sp - 1] = mem[pila[sp - 1]] = pila[sp];
	sp--;
	break;
}
case DIVBC_ori: {
	pila[sp - 1] |= pila[sp];
	sp--;
	break;
}
case DIVBC_xor: {
	pila[sp - 1] ^= pila[sp];
	sp--;
	break;
}
case DIVBC_and: {
	pila[sp - 1] &= pila[sp];
	sp--;
	break;
}
case DIVBC_igu: {
	pila[sp - 1] = pila[sp - 1] == pila[sp];
	sp--;
	break;
}
case DIVBC_dis: {
	pila[sp - 1] = pila[sp - 1] != pila[sp];
	sp--;
	break;
}
case DIVBC_may: {
	pila[sp - 1] = pila[sp - 1] > pila[sp];
	sp--;
	break;
}
case DIVBC_men: {
	pila[sp - 1] = pila[sp - 1] < pila[sp];
	sp--;
	break;
}
case DIVBC_mei: {
	pila[sp - 1] = pila[sp - 1] <= pila[sp];
	sp--;
	break;
}
case DIVBC_mai: {
	pila[sp - 1] = pila[sp - 1] >= pila[sp];
	sp--;
	break;
}
case DIVBC_add: {
	pila[sp - 1] += pila[sp];
	sp--;
	break;
}
case DIVBC_sub: {
	pila[sp - 1] -= pila[sp];
	sp--;
	break;
}
case DIVBC_mul: {
	pila[sp - 1] *= pila[sp];
	sp--;
	break;
}
case DIVBC_div: {
#ifdef DEBUG
	if(pila[sp] == 0) {
		pila[--sp] = 0;

		v_function = -2;

		e(145);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
	}
	else {
		pila[sp - 1] /= pila[sp];
		sp--;
	}
#else
	pila[sp - 1] /= pila[sp];
	sp--;
#endif
	break;
}
case DIVBC_mod: {
#ifdef DEBUG
	if(pila[sp] == 0) {
		pila[--sp] = 0;

		v_function = -2;

		e(145);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
	}
	else {
		pila[sp - 1] %= pila[sp];
		sp--;
	}
#else
	pila[sp - 1] %= pila[sp];
	sp--;
#endif
	break;
}
case DIVBC_neg: {
	pila[sp] = -pila[sp];
	break;
}
case DIVBC_ptr: {
	pila[sp] = mem[pila[sp]];
	break;
}
case DIVBC_not: {
	pila[sp] ^= -1;
	break;
}
case DIVBC_aid: {
	pila[sp] += id;
	break;
}
case DIVBC_cid: {
	pila[++sp] = id;
	break;
}
case DIVBC_rng: {
#ifdef DEBUG
	if(pila[sp] < 0 || pila[sp] > mem[ip]) {
		v_function = -2;

		e(140);

		if(call_to_debug) {
			ip++;

			process_stoped = id;
			return;
		}
	}
	ip++;
#else
	ip++;
#endif
	break;
}
case DIVBC_jmp: {
	ip = mem[ip];

#ifdef DEBUG
	if(reloj > max_reloj) {
		v_function = -2;

		e(142);

		max_reloj = max_process_time + reloj;

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
	}
#endif
	break;
}
case DIVBC_jpf: {
	if(pila[sp--] & 1) {
		ip++;
	}
	else {
		ip = mem[ip];
	}

#ifdef DEBUG
	if(reloj > max_reloj) {
		v_function = -2;

		e(142);

		max_reloj = max_process_time + reloj;

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
	}
#endif
	break;
}
case DIVBC_fun: {
	function();

#ifdef DEBUG
	if(call_to_debug) {
		process_stoped = id;
		return;
	}
#endif
	break;
}
case DIVBC_cal: {
#ifdef DEBUG
	process_exec(id,get_ticks()-oreloj); oreloj=get_ticks();
#endif

	mem[id + _IP] = ip + 1;

	id2 = id;

	if(sp > long_pila) {
		exer(3);
	}

	procesos++;

	ip = mem[ip];
	id = id_start;

//printf("Processes: %d  id:%d p*len:%d id_end: %d\n", procesos, id_end - id, (id_start) + ((procesos - 2) * iloc_len), id_end);

	if((id_start + ((procesos - 2) * iloc_len)) == id_end) {
		id = id_end + iloc_len;
	}

//if((processos-1)*iloc_len>id_end)

	while(mem[id + _Status] && id <= id_end) {
		id += iloc_len;
	}
#ifdef LLPROC
	insert_process(id);
#endif
	if(id > id_end) {
		if(id > imem_max - iloc_len) {
			exer(2);
		}

		id_end = id;
		if(id_end > id_max) {
			id_max = id_end;
		}
	}
	memcpy(&mem[id],
	       &mem[iloc],
	        iloc_pub_len << 2);
	mem[id + _Id] = id;
	if(mem[id + _BigBro] = mem[id2 + _Son]) {
		mem[mem[id + _BigBro] + _SmallBro] = id;
	}
	mem[id2 + _Son]    = id;
	mem[id  + _Father] = mem[id + _Caller] = id2;

	// Es Función
	if(mem[ip + 2] == DIVBC_nop) {
		mem[id + _FCount] = mem[id2 + _FCount] + 1;
	}
#ifdef DEBUG
	else {
		process_level++;
	}
#endif
	break;
}
case DIVBC_ret: {
#ifdef DEBUG
	if(mem[id + _FCount] == 0) {
		process_level--;
	}

	process_exec(id, get_ticks() - oreloj);
	oreloj = get_ticks();
#endif

	sp = mem[id + _Param] - mem[id + _NumPar];
	pila[sp] = id;

	id = mem[id + _Caller];

	elimina_proceso(pila[sp]);

	if(!(id & 1)) {
		goto next_process1;
	}

	ip = mem[id + _IP];
	break;
}
case DIVBC_asp: {
	sp--;
	break;
}
case DIVBC_frm: {
#ifdef DEBUG
	process_exec(id, get_ticks() - oreloj);
	oreloj = get_ticks();
#endif

	sp = mem[id + _Param] - mem[id + _NumPar];

	mem[id + _IP    ] = ip;
next_frm:
	mem[id + _IdScan] = 0;
	mem[id + _BlScan] = 0;

	pila[sp] = id;

	id = mem[id + _Caller];
	if(!(id & 1)) {
		goto next_process1;
	}

	ip = mem[id + _IP];

	// Si era una funcion, duerme al padre.
	if(mem[pila[sp] + _FCount] > 0) {
		// Caller dormido
		mem[pila[sp] + _Caller]++;

		// Deja la pila aparcada en el lugar adecuado
		bp = sp;
		// Y la prerara para retornar al anterior
		sp = mem[id + _Param] - mem[id + _NumPar];

		guarda_pila(id, sp, bp);

		mem[id + _Status] = 3;

		goto next_frm;
	}
	else {
#ifdef DEBUG
		process_level--;
#endif
		mem[pila[sp] + _Caller  ] = 0;
		mem[pila[sp] + _Executed] = 1;
	}
	break;
}
case DIVBC_cbp: {
	mem[id + _NumPar] = mem[ip++];
	mem[id + _Param]  = sp - mem[id + _NumPar] + 1;
	break;
}
case DIVBC_cpa: {
	mem[pila[sp--]] = pila[mem[id + _Param]++];
	break;
}
case DIVBC_typ: {
	mem[id + _Bloque] = mem[ip++];

	inicio_privadas = mem[6];
	break;
}
case DIVBC_pri: {
	memcpy(&mem[id + inicio_privadas],
	       &mem[ip + 1],
	       (mem[ip] - ip - 1) << 2);

	inicio_privadas += (mem[ip] - ip - 1);

	ip = mem[ip];
	break;
}
case DIVBC_cse: {
	if(pila[sp - 1] == pila[sp]) {
		ip++;
	}
	else {
		ip = mem[ip];
	}
	sp--;
	break;
}
case DIVBC_csr: {
	if(pila[sp - 2] >= pila[sp - 1] &&
	   pila[sp - 2] <= pila[sp]) {
		ip++;
	}
	else {
		ip = mem[ip];
	}
	sp -= 2;
	break;
}
case DIVBC_shr: {
	pila[sp - 1] >>= pila[sp];
	sp--;
	break;
}
case DIVBC_shl: {
	pila[sp - 1] <<= pila[sp];
	sp--;
	break;
}
case DIVBC_ipt: {
	pila[sp] = ++mem[pila[sp]];
	break;
}
case DIVBC_pti: {
	pila[sp] = mem[pila[sp]]++;
	break;
}
case DIVBC_dpt: {
	pila[sp] = --mem[pila[sp]];
	break;
	//break;
}
case DIVBC_ptd: {
	pila[sp] = mem[pila[sp]]--;
	break;
}
case DIVBC_ada: {
	pila[sp - 1] = mem[pila[sp - 1]] += pila[sp];
	sp--;
	break;
}
case DIVBC_sua: {
	pila[sp - 1] = mem[pila[sp - 1]] -= pila[sp];
	sp--;
	break;
}
case DIVBC_mua: {
	pila[sp - 1] = mem[pila[sp - 1]] *= pila[sp];
	sp--;
	break;
}
case DIVBC_dia: {
#ifdef DEBUG
	if(pila[sp] == 0) {
		mem[pila[sp - 1]] = 0;

		pila[--sp] = 0;

		v_function = -2;

		e(145);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
#endif
	pila[sp - 1] = mem[pila[sp - 1]] /= pila[sp];
	sp--;
	break;
}
case DIVBC_moa: {
#ifdef DEBUG
	if(pila[sp] == 0) {
		mem[pila[sp - 1]] = 0;

		pila[--sp] = 0;

		v_function = -2;

		e(145);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
#endif
	pila[sp - 1] = mem[pila[sp - 1]] %= pila[sp];
	sp--;
	break;
}
case DIVBC_ana: {
	pila[sp - 1] = mem[pila[sp - 1]] &= pila[sp];
	sp--;
	break;
}
case DIVBC_ora: {
	pila[sp - 1] = mem[pila[sp - 1]] |= pila[sp];
	sp--;
	break;
}
case DIVBC_xoa: {
	pila[sp - 1] = mem[pila[sp - 1]] ^= pila[sp];
	sp--;
	break;
}
case DIVBC_sra: {
	pila[sp - 1] = mem[pila[sp - 1]] >>= pila[sp];
	sp--;
	break;
}
case DIVBC_sla: {
	pila[sp - 1] = mem[pila[sp - 1]] <<= pila[sp];
	sp--;
	break;
}
case DIVBC_par: {
	inicio_privadas += mem[ip++];
	break;
}
case DIVBC_rtf: {
#ifdef DEBUG
	if(mem[id + _FCount] == 0) {
		process_level--;
	}

	process_exec(id, get_ticks() - oreloj);
	oreloj = get_ticks();
#endif

	bp = pila[sp];
	sp = mem[id + _Param] - mem[id + _NumPar];
	pila[sp] = bp;
	bp = id;
	id = mem[id+_Caller];

	elimina_proceso(bp);

	if(!(id & 1)) {
		// Un return de función (tras frame)
		if(id) {
			actualiza_pila(id - 1, pila[sp]);
		}

		goto next_process1;
	}

	ip = mem[id + _IP];
	break;
}
case DIVBC_clo: {
	procesos++;

	id2 = id;
	id  = id_start;
	while(mem[id + _Status] && id <= id_end) {
		id += iloc_len;
	}
	if(id > id_end) {
		if(id > imem_max - iloc_len) {
			exer(2);
		}
		id_end = id;
	}

	memcpy(&mem[id],
	       &mem[id2],
	        iloc_len << 2);

	mem[id + _Id    ] = id;
	mem[id + _IP    ] = ip + 1;
	mem[id + _Caller] = 0;
	if(mem[id + _BigBro] = mem[id2 + _Son]) {
		mem[mem[id + _BigBro] + _SmallBro] = id;
	}
	mem[id  + _SmallBro] = 0;
	mem[id  + _Son     ] = 0;
	mem[id  + _Father  ] = id2;
  mem[id2 + _Son     ] = id;

	id = id2;
	ip = mem[ip];
	break;
}
case DIVBC_frf: {
#ifdef DEBUG
	process_exec(id, get_ticks() - oreloj);
	oreloj = get_ticks();
#endif

	if((mem[id + _Frame] += pila[sp]) >= 100) {
		mem[id + _Frame   ] -= 100;
		mem[id + _Executed]  = 1;
	}

	sp = mem[id + _Param] - mem[id + _NumPar];

	mem[id + _IP    ] = ip;
next_frf:
	mem[id + _IdScan] = 0;
	mem[id + _BlScan] = 0;

	pila[sp] = id;

	id = mem[id + _Caller];
	if(!(id & 1)) {
		goto next_process2;
	}

	ip = mem[id + _IP];

	// Si es una funcion, duerme al padre.
	if(mem[pila[sp] + _FCount] > 0) {
		// Caller dormido
		mem[pila[sp] + _Caller]++;

		// Deja la pila aparcada en el lugar adecuado
		bp = sp;
		// Y la prerara para retornar al anterior
		sp = mem[id + _Param] - mem[id + _NumPar];

		guarda_pila(id, sp, bp);

		mem[id + _Status] = 3;

		goto next_frf;
	}
	else {
		mem[pila[sp] + _Caller] = 0;

#ifdef DEBUG
		process_level--;
#endif
	}
	break;
}
case DIVBC_imp: {
#ifdef DIVDLL
	if((pe[nDLL] = DIV_LoadDll((char *)&mem[mem[ip++]])) == NULL) {
		break;
	}
//	exer(4);
	else {
		nDLL++;
	}
#else
printf("skipping dll %s\n", (char*)&mem[mem[ip++]]);
#endif
	break;
}
case DIVBC_ext: {
#ifdef DIVDLL
	if(ExternDirs[mem[ip]]) {
		call((voidReturnType)ExternDirs[mem[ip++]]);
	}
#endif
	break;
}
case DIVBC_chk: {
#ifdef DEBUG
	if(pila[sp] < id_init || pila[sp] > id_max || pila[sp] != mem[pila[sp]]) {
		v_function = -2;

		e(141);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
	}
#endif
	break;
}
case DIVBC_dbg: {
#ifdef DEBUG
	if(debug_active) {
		for(ibreakpoint = 0; ibreakpoint < max_breakpoint; ibreakpoint++) {
			if(breakpoint[ibreakpoint].line > -1 && abs(breakpoint[ibreakpoint].offset) == ip - 1) {
				break;
			}
		}
		// Se llegó a un breakpoint
		if(ibreakpoint<max_breakpoint) {
			mem[--ip] = breakpoint[ibreakpoint].code;

			breakpoint[ibreakpoint].line = -1;

			call_to_debug  = 1;
			process_stoped = id;

			// Para que se vean los datos del proceso que tenia el break
			ids_old = -1;
			return;
		}

#ifndef TRACE
		deb();

		if(call_to_debug) {
			return;
		}
#endif
	}
#endif
	break;
}
case DIVBC_car2: {
	pila[++sp] = mem[ip++];
	pila[++sp] = mem[ip++];
	break;
}
case DIVBC_car3: {
	pila[++sp] = mem[ip++];
	pila[++sp] = mem[ip++];
	pila[++sp] = mem[ip++];
	break;
}
case DIVBC_car4: {
	pila[++sp] = mem[ip++];
	pila[++sp] = mem[ip++];
	pila[++sp] = mem[ip++];
	pila[++sp] = mem[ip++];
	break;
}
case DIVBC_asiasp: {
	mem[pila[sp - 1]] = pila[sp];
	sp -= 2;
	break;
}
case DIVBC_caraid: {
#ifdef LLPROC
	// check if setting priority or z
	if(mem[ip] == _Priority || mem[ip] == _Z && checklist == 0) {
		checklist = mem[ip]; // save z or p
	}
#endif

	pila[++sp] = mem[ip++] + id;
	break;
}
case DIVBC_carptr: {
	pila[++sp] = mem[mem[ip++]];
	break;
}
case DIVBC_aidptr: {
	pila[sp] = mem[pila[sp] + id];
	break;
}
case DIVBC_caraidptr: {
	pila[++sp] = mem[mem[ip++] + id];
	break;
}
case DIVBC_caraidcpa: {
	mem[mem[ip++] + id] = pila[mem[id + _Param]++];
	break;
}
case DIVBC_addptr: {
	pila[sp - 1] = mem[pila[sp - 1] + pila[sp]];
	sp--;
	break;
}
case DIVBC_funasp: {
	function();
	sp--;

#ifdef DEBUG
	if(call_to_debug) {
		process_stoped = id;
		return;
	}
#endif
	break;
}
case DIVBC_caradd: {
	pila[sp] += mem[ip++];
	break;
}
case DIVBC_caraddptr: {
	pila[sp] = mem[pila[sp] + mem[ip++]];
	break;
}
case DIVBC_carmul: {
	pila[sp] *= mem[ip++];
	break;
}
case DIVBC_carmuladd: {
	pila[sp - 1] += pila[sp] * mem[ip++];
	sp--;
	break;
}
case DIVBC_carasiasp: {
#ifdef LLPROC
	if(checklist > 0 && mem[pila[sp]] != mem[ip] && id > 0) {
		dtemp = mem[pila[sp]];

printf("id: %d old val: %d\n", id, dtemp);

		mem[pila[sp]] = mem[ip++];

		sp--;

		dirty(id);
	}
	//else
#endif
	mem[pila[sp]] = mem[ip++];
	sp--;
	break;
}
case DIVBC_carsub: {
	pila[sp] -= mem[ip++];
	break;
}
case DIVBC_cardiv: {
	pila[sp] /= mem[ip++]; // No hay nunca "cardiv 0"
	break;
}

case DIVBC_ptrchr: {
	pila[sp - 1] = (unsigned int)memb[pila[sp - 1] * 4 + pila[sp]];
	sp--;
	break;
}
case DIVBC_asichr: {
	pila[sp - 2] = (unsigned int)(memb[pila[sp - 2] * 4 + pila[sp - 1]] = (byte)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_iptchr: {
	pila[sp - 1] = (unsigned int)++memb[pila[sp - 1] * 4 + pila[sp]];
	sp--;
	break;
}
case DIVBC_ptichr: {
	pila[sp - 1] = (unsigned int)memb[pila[sp - 1] * 4 + pila[sp]]++;
	sp--;
	break;
}
case DIVBC_dptchr: {
	pila[sp - 1] = (unsigned int)--memb[pila[sp - 1] * 4 + pila[sp]];
	sp--;
	break;
}
case DIVBC_ptdchr: {
	pila[sp - 1] = (unsigned int)memb[pila[sp - 1] * 4 + pila[sp]]--;
	sp--;
	break;
}
case DIVBC_adachr: {
	pila[sp - 2] = (unsigned int)(memb[pila[sp - 2] * 4 + pila[sp - 1]] += (byte)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_suachr: {
	pila[sp - 2] = (unsigned int)(memb[pila[sp - 2] * 4 + pila[sp - 1]] -= (byte)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_muachr: {
	pila[sp - 2] = (unsigned int)(memb[pila[sp - 2] * 4 + pila[sp - 1]] *= (byte)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_diachr: {
#ifdef DEBUG
	if(pila[sp] == 0) {
		memb[pila[sp - 2] * 4 + pila[sp - 1]] = 0;

		sp -= 2;
		pila[sp] = 0;

		v_function = -2;

		e(145);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
#endif
	pila[sp - 2] = (unsigned int)(memb[pila[sp - 2] * 4 + pila[sp - 1]] /= (byte)pila[sp]);
	sp -= 2;
	break;
	//break;
}
case DIVBC_moachr: {
#ifdef DEBUG
	if(pila[sp] == 0) {
		memb[pila[sp - 2] * 4 +pila[sp - 1]] = 0;

		sp -= 2;
		pila[sp] = 0;

		v_function = -2;

		e(145);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
#endif
	pila[sp - 2] = (unsigned int)(memb[pila[sp - 2] * 4 + pila[sp - 1]] %= (byte)pila[sp]);
	sp -= 2;
	break;
	//break;
}
case DIVBC_anachr: {
	pila[sp - 2] = (unsigned int)(memb[pila[sp - 2] * 4 + pila[sp - 1]] &= (byte)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_orachr: {
	pila[sp - 2] = (unsigned int)(memb[pila[sp - 2] * 4 + pila[sp - 1]] |= (byte)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_xoachr: {
	pila[sp - 2] = (unsigned int)(memb[pila[sp - 2] * 4 + pila[sp - 1]] ^= (byte)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_srachr: {
	pila[sp - 2] = (unsigned int)(memb[pila[sp - 2] * 4 + pila[sp - 1]] >>= (byte)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_slachr: {
	pila[sp - 2] = (unsigned int)(memb[pila[sp - 2] * 4 + pila[sp - 1]] <<= (byte)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_cpachr: {
	if((unsigned)pila[mem[id + _Param]] < 256) {
		memb[pila[sp - 1] * 4 + pila[sp]] = (byte)pila[mem[id + _Param]];
	}
	else {
		if(pila[mem[id + _Param]] < imem_max + 258 * 4) {
			memb[pila[sp - 1] * 4 + pila[sp]] = memb[pila[mem[id + _Param]] * 4];
		}
		else {
			memb[pila[sp - 1] * 4 + pila[sp]] = (byte)pila[mem[id + _Param]];
		}
	}

	sp -= 2;

	mem[id + _Param]++;
	break;
}

// Un puntero a cadena menor a 256 se trata siempre como "%c"
case DIVBC_strcpy: {
#ifdef DEBUG
	if((mem[pila[sp - 1] - 1] & 0xFFF00000) != 0xDAD00000) {
		sp--;

		v_function = -2;

		e(164);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
	if((unsigned)pila[sp] > 255) {
		if((mem[pila[sp - 1] - 1] & 0xFFFFF) + 1 < strlen((char *)&mem[pila[sp]])) {
			sp--;

			v_function = -2;

			e(140);

			if(call_to_debug) {
				process_stoped = id;
				return;
			}
			break;
		}
	}
#endif
	if((unsigned)pila[sp] > 255) {
		strcpy((char *)&mem[pila[sp - 1]],
		       (char *)&mem[pila[sp]]);
	}
	else {
		sprintf((char *)&mem[pila[sp - 1]],
		        "%c\0",
		        pila[sp]);
	}
	sp--;
	break;
}
case DIVBC_strfix: {
	if(pila[sp - 1] >= (oo = strlen((char *)&memb[pila[sp - 2] * 4]))) {
		for(; oo < pila[sp - 1]; oo++) {
			memb[pila[sp - 2] * 4 + oo] = ' ';
		}
		memb[pila[sp - 2] * 4 + oo + 1] = 0;
	}
	break;
}
case DIVBC_strcat: {
#ifdef DEBUG
	if((mem[pila[sp - 1] - 1] & 0xFFF00000) != 0xDAD00000) {
		sp--;

		v_function = -2;

		e(164);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
	if((unsigned)pila[sp] > 255) {
		oo = strlen((char *)&mem[pila[sp]]);
	}
	else {
		oo = 1;
	}
	if((mem[pila[sp - 1] - 1] & 0xFFFFF) + 1 < strlen((char *)&mem[pila[sp - 1]]) + oo) {
		sp--;

		v_function = -2;

		e(140);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
#endif
	if((unsigned)pila[sp] > 255) {
		strcat((char *)&mem[pila[sp - 1]],
		       (char *)&mem[pila[sp]]);
	}
	else {
		sprintf((char *)&mem[pila[sp - 1]],
		        "%s%c\0",
		        (char *)&mem[pila[sp - 1]],
		        pila[sp]);
	}
	sp--;
	break;
}
// Strcat "en el aire" (ojo, el aire tiene tambien 0xDAD00402)
case DIVBC_stradd: {
#ifdef DEBUG
	if((unsigned)pila[sp - 1] > 255) {
		oo = strlen((char *)&mem[pila[sp - 1]]);
	}
	else {
		oo = 1;
	}
	if((unsigned)pila[sp] > 255) {
		oo += strlen((char *)&mem[pila[sp]]);
	}
	else {
		oo += 1;
	}
	if(oo >= 1028) {
		sp--;

		v_function = -2;

		e(140);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
#endif

	if((unsigned)pila[sp - 1] > 255) {
		if((unsigned)pila[sp] > 255) {
			sprintf((char *)&mem[nullstring[nstring]],
			        "%s%s\0",
			        (char *)&mem[pila[sp - 1]],
			        (char *)&mem[pila[sp]]);
		}
		else {
			sprintf((char *)&mem[nullstring[nstring]],
			        "%s%c\0",
			        (char *)&mem[pila[sp - 1]],
			        pila[sp]);
		}
	}
	else {
		if((unsigned)pila[sp] > 255) {
			sprintf((char *)&mem[nullstring[nstring]],
			        "%c%s\0",
			        pila[sp - 1],
			        (char *)&mem[pila[sp]]);
		}
		else {
			sprintf((char *)&mem[nullstring[nstring]],
			        "%c%c\0",
			        pila[sp - 1],
			        pila[sp]);
		}
	}

	pila[--sp] = nullstring[nstring];
	nstring = ((nstring + 1) & 3);
	break;
}
// cambio de tamaño "en el aire" (no da error, hace lo que puede)
case DIVBC_strdec: {
	oo = strlen((char *)&mem[pila[sp - 1]]);
	if(oo < 1028) {
		strcpy((char *)&mem[nullstring[nstring]],
		       (char *)&mem[pila[sp - 1]]);

		// Quitar caracteres
		if(pila[sp] > 0) {
			if(pila[sp] >= oo) {
				memb[nullstring[nstring] * 4] = 0;
			}
			else {
				memb[nullstring[nstring] * 4 + oo - pila[sp]] = 0;
			}
		}
		// Añadir (?) caracteres (por homogeneidad)
		else if(pila[sp] < 0) {
			pila[sp] = oo - pila[sp]; // Nueva longitud
			if(pila[sp] > 1025) {
				pila[sp] = 1025;
			}

			for(;oo < pila[sp]; oo++) {
				memb[nullstring[nstring] * 4 + oo] = ' ';
			}
			memb[nullstring[nstring] * 4 + oo] = 0;
		}
	}
	else {
		mem[nullstring[nstring]] = 0;
	}

	pila[--sp] = nullstring[nstring];
	nstring = ((nstring + 1) & 3);
	break;
}
// cambio de tamaño a un string
case DIVBC_strsub: {
	oo = strlen((char *)&mem[pila[sp - 1]]);
#ifdef DEBUG
	if((mem[pila[sp - 1] - 1] & 0xFFF00000) != 0xDAD00000) {
		sp--;

		v_function = -2;

		e(164);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
	if((mem[pila[sp - 1] - 1] & 0xFFFFF) + 1 < oo - pila[sp]) {
		sp--;

		v_function = -2;

		e(140);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
#endif

	// Quitar caracteres
	if(pila[sp] > 0) {
		if(pila[sp] >= oo) {
			memb[pila[sp - 1] * 4] = 0;
		}
		else {
			memb[pila[sp - 1] * 4 + oo - pila[sp]] = 0;
		}
	}
	// Añadir (?) caracteres (por homogeneidad)
	else if(pila[sp] < 0) {
		pila[sp] = oo - pila[sp]; // Nueva longitud
		for(; oo < pila[sp]; oo++) {
			memb[pila[sp - 1] * 4 + oo] = ' ';
		}
		memb[pila[sp - 1] * 4 + oo] = 0;
	}

	sp--;
	break;
}
case DIVBC_strlen: {
	if((unsigned)pila[sp] > 255) {
		pila[sp] = strlen((char *)&mem[pila[sp]]);
	}
	else {
		pila[sp] = 1;
	}
	break;
}
case DIVBC_strigu: {
	if((unsigned)pila[sp - 1] > 255) {
		if((unsigned)pila[sp] > 255) {
			if(strcmp((char *)&mem[pila[sp - 1]],
			          (char *)&mem[pila[sp]])) {
				pila[sp - 1] = 0;
			}
			else {
				pila[sp - 1] = 1;
			}
		}
		else {
			if(strcmp((char *)&mem[pila[sp - 1]],
			          (char *)&pila[sp])) {
				pila[sp - 1] = 0;
			}
			else {
				pila[sp - 1] = 1;
			}
		}
	}
	else {
		if((unsigned)pila[sp ] >255) {
			if(strcmp((char *)&pila[sp - 1],
			          (char *)&mem[pila[sp]])) {
				pila[sp - 1] = 0;
			}
			else {
				pila[sp - 1] = 1;
			}
		}
		else {
			pila[sp - 1] = (pila[sp - 1] == pila[sp]);
		}
	}
	sp--;
	break;
}
case DIVBC_strdis: {
	if((unsigned)pila[sp - 1] > 255) {
		if((unsigned)pila[sp] > 255) {
			if(strcmp((char *)&mem[pila[sp - 1]],
			          (char *)&mem[pila[sp]])) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
		else {
			if(strcmp((char *)&mem[pila[sp - 1]],
			          (char *)&pila[sp])) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
	}
	else {
		if((unsigned)pila[sp] > 255) {
			if(strcmp((char *)&pila[sp - 1],
			          (char *)&mem[pila[sp]])) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
		else {
			pila[sp - 1] = (pila[sp - 1] != pila[sp]);
		}
	}
	sp--;
	break;
	//break;
}
case DIVBC_strmay: {
	if((unsigned)pila[sp - 1] > 255) {
		if((unsigned)pila[sp] > 255) {
			if(strcmp((char *)&mem[pila[sp - 1]],
			          (char *)&mem[pila[sp]]) > 0) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
		else {
			if(strcmp((char *)&mem[pila[sp - 1]],
			          (char *)&pila[sp]) > 0) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
	}
	else {
		if((unsigned)pila[sp] > 255) {
			if(strcmp((char *)&pila[sp - 1],
			          (char *)&mem[pila[sp]]) > 0) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
		else {
			pila[sp - 1] = (pila[sp - 1] > pila[sp]);
		}
	}
	sp--;
	break;
	//break;
}
case DIVBC_strmen: {
	if((unsigned)pila[sp - 1] > 255) {
		if((unsigned)pila[sp] > 255) {
			if(strcmp((char *)&mem[pila[sp - 1]],
			          (char *)&mem[pila[sp]]) < 0) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
		else {
			if(strcmp((char *)&mem[pila[sp - 1]],
			          (char *)&pila[sp]) < 0) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
	}
	else {
		if((unsigned)pila[sp] > 255) {
			if(strcmp((char *)&pila[sp - 1],
			          (char *)&mem[pila[sp]]) < 0) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
		else {
			pila[sp - 1] = (pila[sp - 1] < pila[sp]);
		}
	}
	sp--;
	break;
	//break;
}
case DIVBC_strmei: {
	if((unsigned)pila[sp - 1] > 255) {
		if((unsigned)pila[sp] > 255) {
			if(strcmp((char *)&mem[pila[sp - 1]],
			          (char *)&mem[pila[sp]]) <= 0) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
		else {
			if(strcmp((char *)&mem[pila[sp - 1]],
			          (char *)&pila[sp]) <= 0) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
	}
	else {
		if((unsigned)pila[sp] > 255) {
			if(strcmp((char *)&pila[sp - 1],
			          (char *)&mem[pila[sp]]) <= 0) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
		else {
			pila[sp - 1] = (pila[sp - 1] <= pila[sp]);
		}
	}
	sp--;
	break;
	//break;
}
case DIVBC_strmai: {
	if((unsigned)pila[sp - 1] > 255) {
		if((unsigned)pila[sp] > 255) {
			if(strcmp((char *)&mem[pila[sp - 1]],
			          (char *)&mem[pila[sp]]) >= 0) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
		else {
			if(strcmp((char *)&mem[pila[sp - 1]],
			          (char *)&pila[sp]) >= 0) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
	}
	else {
		if((unsigned)pila[sp] > 255) {
			if(strcmp((char *)&pila[sp - 1],
			          (char *)&mem[pila[sp]]) >= 0) {
				pila[sp - 1] = 1;
			}
			else {
				pila[sp - 1] = 0;
			}
		}
		else {
			pila[sp - 1] = (pila[sp - 1] >= pila[sp]);
		}
	}
	sp--;
	break;
	//break;
}
case DIVBC_cpastr: {
#ifdef DEBUG
	if((mem[pila[sp] - 1] & 0xFFF00000) != 0xDAD00000) {
		sp--;

		mem[id + _Param]++;

		v_function = -2;

		e(164);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
	if((unsigned)pila[mem[id + _Param]] > 255) {
		if((mem[pila[sp] - 1] & 0xFFFFF) + 1 < strlen((char *)&mem[pila[mem[id + _Param]]])) {
			sp--;

			mem[id + _Param]++;

			v_function = -2;

			e(140);

			if(call_to_debug) {
				process_stoped = id;
				return;
			}
			break;
		}
	}
#endif

	if((unsigned)pila[mem[id + _Param]] > 255) {
		strcpy((char *)&mem[pila[sp]],
		       (char *)&mem[pila[mem[id + _Param]]]);
	}
	else {
		sprintf((char *)&mem[pila[sp]],
		        "%c\0",
		        pila[mem[id + _Param]]);
	}
	sp--;

	mem[id + _Param]++;
	break;
}
case DIVBC_ptrwor: {
	pila[sp - 1] = (unsigned int)memw[pila[sp - 1] * 2 + pila[sp]];
	sp--;
	break;
}
case DIVBC_asiwor: {
	pila[sp - 2] = (unsigned int)(memw[pila[sp - 2] * 2 + pila[sp - 1]] = (word)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_iptwor: {
	pila[sp - 1] = (unsigned int)++memw[pila[sp - 1] * 2 + pila[sp]];
	sp--;
	break;
}
case DIVBC_ptiwor: {
	pila[sp - 1] = (unsigned int)memw[pila[sp - 1] * 2 + pila[sp]]++;
	sp--;
	break;
}
case DIVBC_dptwor: {
	pila[sp - 1] = (unsigned int)--memw[pila[sp - 1] * 2 + pila[sp]];
	sp--;
	break;
}
case DIVBC_ptdwor: {
	pila[sp - 1] = (unsigned int)memw[pila[sp - 1] * 2 + pila[sp]]--;
	sp--;
	break;
}
case DIVBC_adawor: {
	pila[sp - 2] = (unsigned int)(memw[pila[sp - 2] * 2 + pila[sp - 1]] += (word)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_suawor: {
	pila[sp - 2] = (unsigned int)(memw[pila[sp - 2] * 2 + pila[sp - 1]] -= (word)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_muawor: {
	pila[sp - 2] = (unsigned int)(memw[pila[sp - 2] * 2 + pila[sp - 1]] *= (word)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_diawor: {
#ifdef DEBUG
	if(pila[sp] == 0) {
		memw[pila[sp - 2] * 2 + pila[sp - 1]] = 0;

		sp -= 2;
		pila[sp] = 0;

		v_function = -2;

		e(145);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
#endif
	pila[sp - 2] = (unsigned int)(memw[pila[sp - 2] * 2 + pila[sp - 1]] /= (word)pila[sp]);
	sp -= 2;
	break;
	//break;
}
case DIVBC_moawor: {
#ifdef DEBUG
	if(pila[sp] == 0) {
		memw[pila[sp - 2] * 2 + pila[sp - 1]] = 0;

		sp -= 2;
		pila[sp] = 0;

		v_function = -2;

		e(145);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
		break;
	}
#endif
	pila[sp - 2] = (unsigned int)(memw[pila[sp - 2] * 2 + pila[sp - 1]] %= (word)pila[sp]);
	sp -= 2;
	break;
	//break;
}
case DIVBC_anawor: {
	pila[sp - 2] = (unsigned int)(memw[pila[sp - 2] * 2 + pila[sp - 1]] &= (word)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_orawor: {
	pila[sp - 2] = (unsigned int)(memw[pila[sp - 2] * 2 + pila[sp - 1]] |= (word)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_xoawor: {
	pila[sp - 2] = (unsigned int)(memw[pila[sp - 2] * 2 + pila[sp - 1]] ^= (word)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_srawor: {
	pila[sp - 2] = (unsigned int)(memw[pila[sp - 2] * 2 + pila[sp - 1]] >>= (word)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_slawor: {
	pila[sp - 2] = (unsigned int)(memw[pila[sp - 2] * 2 + pila[sp-1]] <<= (word)pila[sp]);
	sp -= 2;
	break;
}
case DIVBC_cpawor: {
	if((unsigned)pila[mem[id + _Param]] < 256) {
		memw[pila[sp - 1] * 2 + pila[sp]] = (word)pila[mem[id + _Param]];
	}
	else {
		if(pila[mem[id + _Param]] < imem_max + 258 * 4) {
			memw[pila[sp - 1] * 2 + pila[sp]] = memw[pila[mem[id + _Param]] * 2];
		}
		else {
			memw[pila[sp - 1] * 2 + pila[sp]] = (word)pila[mem[id + _Param]];
		}
	}

	sp -= 2;

	mem[id + _Param]++;
	break;
}
case DIVBC_nul: {
#ifdef DEBUG
	if(!pila[sp]) {
		v_function = -2;

		e(165);

		if(call_to_debug) {
			process_stoped = id;
			return;
		}
	}
#endif
	break;
}
default: {
	fprintf(stdout,
	        "runtime environment Error: unknown instruction! (mem[%d] = %d)\n",
	        ip,
	        mem[ip]);
	fflush(stdout);
	break;
}

/*//═════════════════════════════════════════════════════════════════════════════
//  Cases no utilizados (hasta el 255)
//═════════════════════════════════════════════════════════════════════════════

case 127: case 128: case 129:
case 130: case 131: case 132: case 133: case 134: case 135: case 136: case 137: case 138: case 139:
case 140: case 141: case 142: case 143: case 144: case 145: case 146: case 147: case 148: case 149:
case 150: case 151: case 152: case 153: case 154: case 155: case 156: case 157: case 158: case 159:
case 160: case 161: case 162: case 163: case 164: case 165: case 166: case 167: case 168: case 169:
case 170: case 171: case 172: case 173: case 174: case 175: case 176: case 177: case 178: case 179:
case 180: case 181: case 182: case 183: case 184: case 185: case 186: case 187: case 188: case 189:
case 190: case 191: case 192: case 193: case 194: case 195: case 196: case 197: case 198: case 199:
case 200: case 201: case 202: case 203: case 204: case 205: case 206: case 207: case 208: case 209:
case 210: case 211: case 212: case 213: case 214: case 215: case 216: case 217: case 218: case 219:
case 220: case 221: case 222: case 223: case 224: case 225: case 226: case 227: case 228: case 229:
case 230: case 231: case 232: case 233: case 234: case 235: case 236: case 237: case 238: case 239:
case 240: case 241: case 242: case 243: case 244: case 245: case 246: case 247: case 248: case 249:
case 250: case 251: case 252: case 253: case 254: case 255: break;*/

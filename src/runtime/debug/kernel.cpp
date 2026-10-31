
//═════════════════════════════════════════════════════════════════════════════
// Core of program execution in DIV
//═════════════════════════════════════════════════════════════════════════════

case lnop: break;
case lcar: opstack[++sp]=mem[ip++]; break;
case lasi: opstack[sp-1]=mem[opstack[sp-1]]=opstack[sp]; sp--; break;
case lori: opstack[sp-1]|=opstack[sp]; sp--; break;
case lxor: opstack[sp-1]^=opstack[sp]; sp--; break;
case land: opstack[sp-1]&=opstack[sp]; sp--; break;
case ligu: opstack[sp-1]=opstack[sp-1]==opstack[sp]; sp--; break;
case ldis: opstack[sp-1]=opstack[sp-1]!=opstack[sp]; sp--; break;
case lmay: opstack[sp-1]=opstack[sp-1]>opstack[sp]; sp--; break;
case lmen: opstack[sp-1]=opstack[sp-1]<opstack[sp]; sp--; break;
case lmei: opstack[sp-1]=opstack[sp-1]<=opstack[sp]; sp--; break;
case lmai: opstack[sp-1]=opstack[sp-1]>=opstack[sp]; sp--; break;
case ladd: opstack[sp-1]+=opstack[sp]; sp--; break;
case lsub: opstack[sp-1]-=opstack[sp]; sp--; break;
case lmul: opstack[sp-1]*=opstack[sp]; sp--; break;
case ldiv:
  #ifdef DEBUG
    if (opstack[sp]==0) {
      opstack[--sp]=0; v_function=-2; e(145);
      if (call_to_debug) { process_stoped=id; return; }
    } else { opstack[sp-1]/=opstack[sp]; sp--; }
  #else
    opstack[sp-1]/=opstack[sp]; sp--;
  #endif
  break;
case lmod:
  #ifdef DEBUG
    if (opstack[sp]==0) {
      opstack[--sp]=0; v_function=-2; e(145);
      if (call_to_debug) { process_stoped=id; return; }
    } else { opstack[sp-1]%=opstack[sp]; sp--; }
  #else
    opstack[sp-1]%=opstack[sp]; sp--;
  #endif
  break;
case lneg: opstack[sp]=-opstack[sp]; break;
case lptr: opstack[sp]=mem[opstack[sp]]; break;
case lnot: opstack[sp]^=-1; break;
case laid: opstack[sp]+=id; break;
case lcid: opstack[++sp]=id; break;
case lrng:
  #ifdef DEBUG
    if (opstack[sp]<0 || opstack[sp]>mem[ip]) {
      v_function=-2; e(140);
      if (call_to_debug) { ip++; process_stoped=id; return; }
    } ip++;
  #else
    ip++;
  #endif
  break;
case ljmp:
  ip=mem[ip];
  #ifdef DEBUG
    if (clock_ticks>max_clock) {
      v_function=-2; e(142); max_clock=max_process_time+clock_ticks;
      if (call_to_debug) { process_stoped=id; return; }
    }
  #endif
  break;
case ljpf:
  if (opstack[sp--]&1) ip++; else ip=mem[ip];
  #ifdef DEBUG
    if (clock_ticks>max_clock) {
      v_function=-2; e(142); max_clock=max_process_time+clock_ticks;
      if (call_to_debug) { process_stoped=id; return; }
    }
  #endif
  break;
case lfun:
  function();
  #ifdef DEBUG
    if (call_to_debug) { process_stoped=id; return; }
  #endif
  break;
case lcal:
  #ifdef DEBUG
  process_exec(id,get_ticks()-oclock); oclock=get_ticks();
  #endif
  mem[id+_IP]=ip+1; id2=id; if (sp>long_stack) exer(3);
  processes++; ip=mem[ip]; id=id_start;

//printf("Processes: %d  id:%d p*len:%d id_end: %d\n",processes,id_end-id,(id_start)+((processes-2)*iloc_len),id_end);


if((id_start+((processes-2)*iloc_len)) == id_end)
	id=id_end+iloc_len;


//if((processos-1)*iloc_len>id_end)


  while (mem[id+_Status] && id<=id_end) 
	id+=iloc_len;
#ifdef LLPROC
  insert_process(id);
#endif
  if (id>id_end) { if (id>imem_max-iloc_len) exer(2); id_end=id; if(id_end>id_max) id_max = id_end;}
  memcpy(&mem[id],&mem[iloc],iloc_pub_len<<2);
  mem[id+_Id]=id;
  if (mem[id+_BigBro]=mem[id2+_Son]) mem[mem[id+_BigBro]+_SmallBro]=id;
  mem[id2+_Son]=id; mem[id+_Father]=mem[id+_Caller]=id2;
  if (mem[ip+2]==lnop) mem[id+_FCount]=mem[id2+_FCount]+1; // Function
  #ifdef DEBUG
  else process_level++;
  #endif
  break;
case lret:
  #ifdef DEBUG
  if (mem[id+_FCount]==0) process_level--;
  process_exec(id,get_ticks()-oclock); oclock=get_ticks();
  #endif
  sp=mem[id+_Param]-mem[id+_NumParams];
  opstack[sp]=id; id=mem[id+_Caller];
  delete_process(opstack[sp]);
  if (!(id&1)) goto next_process1;
  ip=mem[id+_IP];
  break;
case lasp:
  sp--;
  break;
case lfrm:
  #ifdef DEBUG
  process_exec(id,get_ticks()-oclock); oclock=get_ticks();
  #endif
  sp=mem[id+_Param]-mem[id+_NumParams];
  mem[id+_IP]=ip;
  next_frm:
  mem[id+_IdScan]=0; mem[id+_BlScan]=0;
  opstack[sp]=id; id=mem[id+_Caller];
  if (!(id&1)) goto next_process1;
  ip=mem[id+_IP];
  if (mem[opstack[sp]+_FCount]>0) { // If it was a function, put the father to sleep.
    mem[opstack[sp]+_Caller]++; // Caller sleeping
    bp=sp; // Leave the stack parked in the right place
    sp=mem[id+_Param]-mem[id+_NumParams]; // And prepare it to return to the previous one
    save_stack(id,sp,bp);
    mem[id+_Status]=3; goto next_frm;
  } else {
    #ifdef DEBUG
    process_level--;
    #endif
    mem[opstack[sp]+_Caller]=0;
    mem[opstack[sp]+_Executed]=1;
  } break;
case lcbp:
  mem[id+_NumParams]=mem[ip++];
  mem[id+_Param]=sp-mem[id+_NumParams]+1;
  break;
case lcpa: mem[opstack[sp--]]=opstack[mem[id+_Param]++]; break;
case ltyp: mem[id+_Block]=mem[ip++]; private_start=mem[6]; break;
case lpri:
  memcpy(&mem[id+private_start],&mem[ip+1],(mem[ip]-ip-1)<<2);
  private_start+=(mem[ip]-ip-1); ip=mem[ip]; break;
case lcse:
  if (opstack[sp-1]==opstack[sp]) ip++; else ip=mem[ip];
  sp--; break;
case lcsr:
  if (opstack[sp-2]>=opstack[sp-1] && opstack[sp-2]<=opstack[sp]) ip++;
  else ip=mem[ip]; sp-=2; break;
case lshr: opstack[sp-1]>>=opstack[sp]; sp--; break;
case lshl: opstack[sp-1]<<=opstack[sp]; sp--; break;
case lipt:
		opstack[sp]=++mem[opstack[sp]];
	break;
case lpti:
		opstack[sp]=mem[opstack[sp]]++;
	break;
case ldpt:
		opstack[sp]=--mem[opstack[sp]]; break;
	break;
case lptd:
		opstack[sp]=mem[opstack[sp]]--;
	break;
case lada: opstack[sp-1]=mem[opstack[sp-1]]+=opstack[sp]; sp--; break;
case lsua: opstack[sp-1]=mem[opstack[sp-1]]-=opstack[sp]; sp--; break;
case lmua: opstack[sp-1]=mem[opstack[sp-1]]*=opstack[sp]; sp--; break;
case ldia:
  #ifdef DEBUG
    if (opstack[sp]==0) {
      mem[opstack[sp-1]]=0;
      opstack[--sp]=0; v_function=-2; e(145);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
  #endif
  opstack[sp-1]=mem[opstack[sp-1]]/=opstack[sp]; sp--;
  break;
case lmoa:
  #ifdef DEBUG
    if (opstack[sp]==0) {
      mem[opstack[sp-1]]=0;
      opstack[--sp]=0; v_function=-2; e(145);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
  #endif
  opstack[sp-1]=mem[opstack[sp-1]]%=opstack[sp]; sp--;
  break;
case lana: opstack[sp-1]=mem[opstack[sp-1]]&=opstack[sp]; sp--; break;
case lora: opstack[sp-1]=mem[opstack[sp-1]]|=opstack[sp]; sp--; break;
case lxoa: opstack[sp-1]=mem[opstack[sp-1]]^=opstack[sp]; sp--; break;
case lsra: opstack[sp-1]=mem[opstack[sp-1]]>>=opstack[sp]; sp--; break;
case lsla: opstack[sp-1]=mem[opstack[sp-1]]<<=opstack[sp]; sp--; break;
case lpar: private_start+=mem[ip++]; break;
case lrtf:
  #ifdef DEBUG
  if (mem[id+_FCount]==0) process_level--;
  process_exec(id,get_ticks()-oclock); oclock=get_ticks();
  #endif
  bp=opstack[sp];
  sp=mem[id+_Param]-mem[id+_NumParams];
  opstack[sp]=bp;
  bp=id; id=mem[id+_Caller];
  delete_process(bp);
  if (!(id&1)) {
    if (id) update_stack(id-1,opstack[sp]); // A function return (after frame)
    goto next_process1;
  }
  ip=mem[id+_IP];
  break;
case lclo:
  processes++; id2=id; id=id_start;
  while (mem[id+_Status] && id<=id_end) id+=iloc_len;
  if (id>id_end) { if (id>imem_max-iloc_len) exer(2); id_end=id; }
  memcpy(&mem[id],&mem[id2],iloc_len<<2);
  mem[id+_Id]=id; mem[id+_IP]=ip+1; mem[id+_Caller]=0;
  if (mem[id+_BigBro]=mem[id2+_Son]) mem[mem[id+_BigBro]+_SmallBro]=id;
  mem[id+_SmallBro]=0; mem[id+_Son]=0;
  mem[id2+_Son]=id; mem[id+_Father]=id2;
  id=id2; ip=mem[ip]; break;
case lfrf:
  #ifdef DEBUG
  process_exec(id,get_ticks()-oclock); oclock=get_ticks();
  #endif
  if ((mem[id+_Frame]+=opstack[sp])>=100) {
    mem[id+_Frame]-=100; mem[id+_Executed]=1;
  }
  sp=mem[id+_Param]-mem[id+_NumParams];
  mem[id+_IP]=ip;
  next_frf:
  mem[id+_IdScan]=0; mem[id+_BlScan]=0;
  opstack[sp]=id; id=mem[id+_Caller];
  if (!(id&1)) goto next_process2;
  ip=mem[id+_IP];
  if (mem[opstack[sp]+_FCount]>0) { // If it is a function, put the father to sleep.
    mem[opstack[sp]+_Caller]++; // Caller sleeping
    bp=sp; // Leave the stack parked in the right place
    sp=mem[id+_Param]-mem[id+_NumParams]; // And prepare it to return to the previous one
    save_stack(id,sp,bp);
    mem[id+_Status]=3; goto next_frf;
  } else {
    mem[opstack[sp]+_Caller]=0;
    #ifdef DEBUG
    process_level--;
    #endif
  } break;
case limp:
#ifdef DIVDLL
    if ((pe[nDLL]=DIV_LoadDll((char*)&mem[mem[ip++]]))==NULL)
    break;
//      exer(4);
    else
      nDLL++;
#else
printf("skipping dll %s\n",(char*)&mem[mem[ip++]]);
#endif
  break;
case lext:
#ifdef DIVDLL
if(ExternDirs[mem[ip]])
    call((voidReturnType)ExternDirs[mem[ip++]]);
#endif
  break;
case lchk:
  #ifdef DEBUG
    if (opstack[sp]<id_init || opstack[sp]>id_max || opstack[sp]!=mem[opstack[sp]]) {
      v_function=-2; e(141);
      if (call_to_debug) { process_stoped=id; return; }
    }
  #endif
  break;
case ldbg:
  #ifdef DEBUG
    if (debug_active) {
      for (ibreakpoint=0;ibreakpoint<max_breakpoint;ibreakpoint++)
        if (breakpoint[ibreakpoint].line>-1 && abs(breakpoint[ibreakpoint].offset)==ip-1) break;
      if (ibreakpoint<max_breakpoint) { // A breakpoint was reached
        mem[--ip]=breakpoint[ibreakpoint].code;
        breakpoint[ibreakpoint].line=-1;
        call_to_debug=1; process_stoped=id;
        ids_old=-1; // So that the data of the process that had the break are shown
        return;
      }
      #ifndef TRACE
        deb(); if (call_to_debug) return;
      #endif
    }
  #endif
  break;

case lcar2: opstack[++sp]=mem[ip++]; opstack[++sp]=mem[ip++]; break;
case lcar3: opstack[++sp]=mem[ip++]; opstack[++sp]=mem[ip++]; opstack[++sp]=mem[ip++]; break;
case lcar4: opstack[++sp]=mem[ip++]; opstack[++sp]=mem[ip++]; opstack[++sp]=mem[ip++]; opstack[++sp]=mem[ip++]; break;
case lasiasp:
  mem[opstack[sp-1]]=opstack[sp]; sp-=2;
  break;
case lcaraid: 
#ifdef LLPROC
	// check if setting priority or z
	if (mem[ip]==_Priority || mem[ip]==_Z && checklist==0) {
		checklist=mem[ip]; // save z or p
	}
#endif

	opstack[++sp]=mem[ip++]+id; 
	break;
case lcarptr: opstack[++sp]=mem[mem[ip++]]; break;
case laidptr: opstack[sp]=mem[opstack[sp]+id]; break;
case lcaraidptr: opstack[++sp]=mem[mem[ip++]+id]; break;
case lcaraidcpa: mem[mem[ip++]+id]=opstack[mem[id+_Param]++]; break;
case laddptr: opstack[sp-1]=mem[opstack[sp-1]+opstack[sp]]; sp--; break;
case lfunasp:
  function();
  sp--;
  #ifdef DEBUG
    if (call_to_debug) { process_stoped=id; return; }
  #endif
  break;
case lcaradd: opstack[sp]+=mem[ip++]; break;
case lcaraddptr: opstack[sp]=mem[opstack[sp]+mem[ip++]]; break;
case lcarmul: opstack[sp]*=mem[ip++]; break;
case lcarmuladd: opstack[sp-1]+=opstack[sp]*mem[ip++]; sp--; break;
case lcarasiasp:
#ifdef LLPROC
	if(checklist>0 && mem[opstack[sp]]!=mem[ip] && id>0) {
		dtemp=mem[opstack[sp]];
		printf("id: %d old val: %d\n",id, dtemp);
		mem[opstack[sp]]=mem[ip++]; sp--;
		dirty(id);
	} else
#endif
	  mem[opstack[sp]]=mem[ip++]; sp--;
  break;
case lcarsub: opstack[sp]-=mem[ip++]; break;
case lcardiv: opstack[sp]/=mem[ip++]; break; // There is never a "cardiv 0"

case lptrchr:
  opstack[sp-1]=(unsigned int)memb[opstack[sp-1]*4+opstack[sp]]; sp--; break;
case lasichr:
  opstack[sp-2]=(unsigned int)(memb[opstack[sp-2]*4+opstack[sp-1]]=(byte)opstack[sp]); sp-=2; break;
case liptchr:
	opstack[sp-1]=(unsigned int)++memb[opstack[sp-1]*4+opstack[sp]]; sp--; break;
case lptichr:
	opstack[sp-1]=(unsigned int)memb[opstack[sp-1]*4+opstack[sp]]++;	sp--; break;
case ldptchr:
	opstack[sp-1]=(unsigned int)--memb[opstack[sp-1]*4+opstack[sp]]; sp--; break;
case lptdchr:
  opstack[sp-1]=(unsigned int)memb[opstack[sp-1]*4+opstack[sp]]--; sp--; break;
case ladachr:
  opstack[sp-2]=(unsigned int)(memb[opstack[sp-2]*4+opstack[sp-1]]+=(byte)opstack[sp]); sp-=2; break;
case lsuachr:
  opstack[sp-2]=(unsigned int)(memb[opstack[sp-2]*4+opstack[sp-1]]-=(byte)opstack[sp]); sp-=2; break;
case lmuachr:
  opstack[sp-2]=(unsigned int)(memb[opstack[sp-2]*4+opstack[sp-1]]*=(byte)opstack[sp]); sp-=2; break;
case ldiachr:
  #ifdef DEBUG
    if (opstack[sp]==0) {
      memb[opstack[sp-2]*4+opstack[sp-1]]=0;
      sp-=2; opstack[sp]=0;
      v_function=-2; e(145);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
  #endif
  opstack[sp-2]=(unsigned int)(memb[opstack[sp-2]*4+opstack[sp-1]]/=(byte)opstack[sp]); sp-=2; break;
  break;
case lmoachr:
  #ifdef DEBUG
    if (opstack[sp]==0) {
      memb[opstack[sp-2]*4+opstack[sp-1]]=0;
      sp-=2; opstack[sp]=0;
      v_function=-2; e(145);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
  #endif
  opstack[sp-2]=(unsigned int)(memb[opstack[sp-2]*4+opstack[sp-1]]%=(byte)opstack[sp]); sp-=2; break;
  break;
case lanachr:
  opstack[sp-2]=(unsigned int)(memb[opstack[sp-2]*4+opstack[sp-1]]&=(byte)opstack[sp]); sp-=2; break;
case lorachr:
  opstack[sp-2]=(unsigned int)(memb[opstack[sp-2]*4+opstack[sp-1]]|=(byte)opstack[sp]); sp-=2; break;
case lxoachr:
  opstack[sp-2]=(unsigned int)(memb[opstack[sp-2]*4+opstack[sp-1]]^=(byte)opstack[sp]); sp-=2; break;
case lsrachr:
  opstack[sp-2]=(unsigned int)(memb[opstack[sp-2]*4+opstack[sp-1]]>>=(byte)opstack[sp]); sp-=2; break;
case lslachr:
  opstack[sp-2]=(unsigned int)(memb[opstack[sp-2]*4+opstack[sp-1]]<<=(byte)opstack[sp]); sp-=2; break;
case lcpachr:
  if ((unsigned)opstack[mem[id+_Param]]<256) {
    memb[opstack[sp-1]*4+opstack[sp]]=(byte)opstack[mem[id+_Param]];
  } else {
    if (opstack[mem[id+_Param]]<imem_max+258*4)
      memb[opstack[sp-1]*4+opstack[sp]]=memb[opstack[mem[id+_Param]]*4];
    else
      memb[opstack[sp-1]*4+opstack[sp]]=(byte)opstack[mem[id+_Param]];
  } sp-=2; mem[id+_Param]++; break;

// A string pointer below 256 is always treated as "%c"

case lstrcpy:
  #ifdef DEBUG
    if ((mem[opstack[sp-1]-1]&0xFFF00000)!=0xDAD00000) {
      sp--; v_function=-2; e(164);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
    if ((unsigned)opstack[sp]>255) if ((mem[opstack[sp-1]-1]&0xFFFFF)+1<strlen((char*)&mem[opstack[sp]])) {
      sp--; v_function=-2; e(140);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
  #endif
  if ((unsigned)opstack[sp]>255) strcpy((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]]);
  else sprintf((char*)&mem[opstack[sp-1]],"%c\0",opstack[sp]);
  sp--;
  break;

case lstrfix:
  if (opstack[sp-1]>=(oo=strlen((char *)&memb[opstack[sp-2]*4]))) {
    for (;oo<opstack[sp-1];oo++) {
      memb[opstack[sp-2]*4+oo]=' ';
    } memb[opstack[sp-2]*4+oo+1]=0;
  } break;

case lstrcat:
  #ifdef DEBUG
    if ((mem[opstack[sp-1]-1]&0xFFF00000)!=0xDAD00000) {
      sp--; v_function=-2; e(164);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
    if ((unsigned)opstack[sp]>255) oo=strlen((char*)&mem[opstack[sp]]); else oo=1;
    if ((mem[opstack[sp-1]-1]&0xFFFFF)+1<strlen((char*)&mem[opstack[sp-1]])+oo) {
      sp--; v_function=-2; e(140);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
  #endif
  if ((unsigned)opstack[sp]>255) strcat((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]]);
  else sprintf((char*)&mem[opstack[sp-1]],"%s%c\0",(char*)&mem[opstack[sp-1]],opstack[sp]);
  sp--;
  break;

case lstradd: // Strcat "in the air" (careful, the air also has 0xDAD00402)
  #ifdef DEBUG
    if ((unsigned)opstack[sp-1]>255) oo=strlen((char*)&mem[opstack[sp-1]]); else oo=1;
    if ((unsigned)opstack[sp]>255) oo+=strlen((char*)&mem[opstack[sp]]); else oo+=1;
    if (oo>=1028) {
      sp--; v_function=-2; e(140);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
  #endif
  if ((unsigned)opstack[sp-1]>255) {
    if ((unsigned)opstack[sp]>255) {
      sprintf((char*)&mem[nullstring[nstring]],"%s%s\0",(char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]]);
    } else {
      sprintf((char*)&mem[nullstring[nstring]],"%s%c\0",(char*)&mem[opstack[sp-1]],opstack[sp]);
    }
  } else {
    if ((unsigned)opstack[sp]>255) {
      sprintf((char*)&mem[nullstring[nstring]],"%c%s\0",opstack[sp-1],(char*)&mem[opstack[sp]]);
    } else {
      sprintf((char*)&mem[nullstring[nstring]],"%c%c\0",opstack[sp-1],opstack[sp]);
    }
  }
  opstack[--sp]=nullstring[nstring];
  nstring=((nstring+1)&3);
  break;

case lstrdec: // resize "in the air" (gives no error, does what it can)
  oo=strlen((char*)&mem[opstack[sp-1]]);
  if (oo<1028) {
    strcpy((char*)&mem[nullstring[nstring]],(char*)&mem[opstack[sp-1]]);
    if (opstack[sp]>0) { // Remove characters
      if (opstack[sp]>=oo) memb[nullstring[nstring]*4]=0;
      else memb[nullstring[nstring]*4+oo-opstack[sp]]=0;
    } else if (opstack[sp]<0) { // Add (?) characters (for consistency)
      opstack[sp]=oo-opstack[sp]; // New length
      if (opstack[sp]>1025) opstack[sp]=1025;
      for (;oo<opstack[sp];oo++) {
        memb[nullstring[nstring]*4+oo]=' ';
      } memb[nullstring[nstring]*4+oo]=0;
    }
  } else mem[nullstring[nstring]]=0;
  opstack[--sp]=nullstring[nstring];
  nstring=((nstring+1)&3);
  break;

case lstrsub: // resize a string
  oo=strlen((char*)&mem[opstack[sp-1]]);
  #ifdef DEBUG
    if ((mem[opstack[sp-1]-1]&0xFFF00000)!=0xDAD00000) {
      sp--; v_function=-2; e(164);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
    if ((mem[opstack[sp-1]-1]&0xFFFFF)+1<oo-opstack[sp]) {
      sp--; v_function=-2; e(140);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
  #endif
  if (opstack[sp]>0) { // Remove characters
    if (opstack[sp]>=oo) memb[opstack[sp-1]*4]=0;
    else memb[opstack[sp-1]*4+oo-opstack[sp]]=0;
  } else if (opstack[sp]<0) { // Add (?) characters (for consistency)
    opstack[sp]=oo-opstack[sp]; // New length
    for (;oo<opstack[sp];oo++) {
      memb[opstack[sp-1]*4+oo]=' ';
    } memb[opstack[sp-1]*4+oo]=0;
  } sp--;
  break;

case lstrlen:
  if ((unsigned)opstack[sp]>255) opstack[sp]=strlen((char*)&mem[opstack[sp]]); else opstack[sp]=1;
  break;

case lstrigu:
  if ((unsigned)opstack[sp-1]>255) {
    if ((unsigned)opstack[sp]>255) {
      if (strcmp((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]])) opstack[sp-1]=0; else opstack[sp-1]=1;
    } else {
      if (strcmp((char*)&mem[opstack[sp-1]],(char*)&opstack[sp])) opstack[sp-1]=0; else opstack[sp-1]=1;
    }
  } else {
    if ((unsigned)opstack[sp]>255) {
      if (strcmp((char*)&opstack[sp-1],(char*)&mem[opstack[sp]])) opstack[sp-1]=0; else opstack[sp-1]=1;
    } else {
      opstack[sp-1]=(opstack[sp-1]==opstack[sp]);
    }
  } sp--; break;

case lstrdis:
  if ((unsigned)opstack[sp-1]>255) {
    if ((unsigned)opstack[sp]>255) {
      if (strcmp((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]])) opstack[sp-1]=1; else opstack[sp-1]=0;
    } else {
      if (strcmp((char*)&mem[opstack[sp-1]],(char*)&opstack[sp])) opstack[sp-1]=1; else opstack[sp-1]=0;
    }
  } else {
    if ((unsigned)opstack[sp]>255) {
      if (strcmp((char*)&opstack[sp-1],(char*)&mem[opstack[sp]])) opstack[sp-1]=1; else opstack[sp-1]=0;
    } else {
      opstack[sp-1]=(opstack[sp-1]!=opstack[sp]);
    }
  } sp--; break;
  break;

case lstrmay:
  if ((unsigned)opstack[sp-1]>255) {
    if ((unsigned)opstack[sp]>255) {
      if (strcmp((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]])>0) opstack[sp-1]=1; else opstack[sp-1]=0;
    } else {
      if (strcmp((char*)&mem[opstack[sp-1]],(char*)&opstack[sp])>0) opstack[sp-1]=1; else opstack[sp-1]=0;
    }
  } else {
    if ((unsigned)opstack[sp]>255) {
      if (strcmp((char*)&opstack[sp-1],(char*)&mem[opstack[sp]])>0) opstack[sp-1]=1; else opstack[sp-1]=0;
    } else {
      opstack[sp-1]=(opstack[sp-1]>opstack[sp]);
    }
  } sp--; break;
  break;

case lstrmen:
  if ((unsigned)opstack[sp-1]>255) {
    if ((unsigned)opstack[sp]>255) {
      if (strcmp((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]])<0) opstack[sp-1]=1; else opstack[sp-1]=0;
    } else {
      if (strcmp((char*)&mem[opstack[sp-1]],(char*)&opstack[sp])<0) opstack[sp-1]=1; else opstack[sp-1]=0;
    }
  } else {
    if ((unsigned)opstack[sp]>255) {
      if (strcmp((char*)&opstack[sp-1],(char*)&mem[opstack[sp]])<0) opstack[sp-1]=1; else opstack[sp-1]=0;
    } else {
      opstack[sp-1]=(opstack[sp-1]<opstack[sp]);
    }
  } sp--; break;
  break;

case lstrmei:
  if ((unsigned)opstack[sp-1]>255) {
    if ((unsigned)opstack[sp]>255) {
      if (strcmp((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]])<=0) opstack[sp-1]=1; else opstack[sp-1]=0;
    } else {
      if (strcmp((char*)&mem[opstack[sp-1]],(char*)&opstack[sp])<=0) opstack[sp-1]=1; else opstack[sp-1]=0;
    }
  } else {
    if ((unsigned)opstack[sp]>255) {
      if (strcmp((char*)&opstack[sp-1],(char*)&mem[opstack[sp]])<=0) opstack[sp-1]=1; else opstack[sp-1]=0;
    } else {
      opstack[sp-1]=(opstack[sp-1]<=opstack[sp]);
    }
  } sp--; break;
  break;

case lstrmai:
  if ((unsigned)opstack[sp-1]>255) {
    if ((unsigned)opstack[sp]>255) {
      if (strcmp((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]])>=0) opstack[sp-1]=1; else opstack[sp-1]=0;
    } else {
      if (strcmp((char*)&mem[opstack[sp-1]],(char*)&opstack[sp])>=0) opstack[sp-1]=1; else opstack[sp-1]=0;
    }
  } else {
    if ((unsigned)opstack[sp]>255) {
      if (strcmp((char*)&opstack[sp-1],(char*)&mem[opstack[sp]])>=0) opstack[sp-1]=1; else opstack[sp-1]=0;
    } else {
      opstack[sp-1]=(opstack[sp-1]>=opstack[sp]);
    }
  } sp--; break;
  break;

case lcpastr:
  #ifdef DEBUG
    if ((mem[opstack[sp]-1]&0xFFF00000)!=0xDAD00000) {
      sp--; mem[id+_Param]++; v_function=-2; e(164);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
    if ((unsigned)opstack[mem[id+_Param]]>255) {
      if ((mem[opstack[sp]-1]&0xFFFFF)+1<strlen((char*)&mem[opstack[mem[id+_Param]]])) {
        sp--; mem[id+_Param]++; v_function=-2; e(140);
        if (call_to_debug) { process_stoped=id; return; }
        break;
      }
    }
  #endif
  if ((unsigned)opstack[mem[id+_Param]]>255)
    strcpy((char*)&mem[opstack[sp]],(char*)&mem[opstack[mem[id+_Param]]]);
  else sprintf((char*)&mem[opstack[sp]],"%c\0",opstack[mem[id+_Param]]);
  sp--; mem[id+_Param]++;
  break;

case lptrwor:
  opstack[sp-1]=(unsigned int)memw[opstack[sp-1]*2+opstack[sp]]; sp--; break;
case lasiwor:
  opstack[sp-2]=(unsigned int)(memw[opstack[sp-2]*2+opstack[sp-1]]=(word)opstack[sp]); sp-=2; break;
case liptwor:
	opstack[sp-1]=(unsigned int)++memw[opstack[sp-1]*2+opstack[sp]]; sp--; break;
case lptiwor:
	opstack[sp-1]=(unsigned int)memw[opstack[sp-1]*2+opstack[sp]]++;	sp--; break;
case ldptwor:
	opstack[sp-1]=(unsigned int)--memw[opstack[sp-1]*2+opstack[sp]]; sp--; break;
case lptdwor:
  opstack[sp-1]=(unsigned int)memw[opstack[sp-1]*2+opstack[sp]]--; sp--; break;
case ladawor:
  opstack[sp-2]=(unsigned int)(memw[opstack[sp-2]*2+opstack[sp-1]]+=(word)opstack[sp]); sp-=2; break;
case lsuawor:
  opstack[sp-2]=(unsigned int)(memw[opstack[sp-2]*2+opstack[sp-1]]-=(word)opstack[sp]); sp-=2; break;
case lmuawor:
  opstack[sp-2]=(unsigned int)(memw[opstack[sp-2]*2+opstack[sp-1]]*=(word)opstack[sp]); sp-=2; break;
case ldiawor:
  #ifdef DEBUG
    if (opstack[sp]==0) {
      memw[opstack[sp-2]*2+opstack[sp-1]]=0;
      sp-=2; opstack[sp]=0;
      v_function=-2; e(145);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
  #endif
  opstack[sp-2]=(unsigned int)(memw[opstack[sp-2]*2+opstack[sp-1]]/=(word)opstack[sp]); sp-=2; break;
  break;
case lmoawor:
  #ifdef DEBUG
    if (opstack[sp]==0) {
      memw[opstack[sp-2]*2+opstack[sp-1]]=0;
      sp-=2; opstack[sp]=0;
      v_function=-2; e(145);
      if (call_to_debug) { process_stoped=id; return; }
      break;
    }
  #endif
  opstack[sp-2]=(unsigned int)(memw[opstack[sp-2]*2+opstack[sp-1]]%=(word)opstack[sp]); sp-=2; break;
  break;
case lanawor:
  opstack[sp-2]=(unsigned int)(memw[opstack[sp-2]*2+opstack[sp-1]]&=(word)opstack[sp]); sp-=2; break;
case lorawor:
  opstack[sp-2]=(unsigned int)(memw[opstack[sp-2]*2+opstack[sp-1]]|=(word)opstack[sp]); sp-=2; break;
case lxoawor:
  opstack[sp-2]=(unsigned int)(memw[opstack[sp-2]*2+opstack[sp-1]]^=(word)opstack[sp]); sp-=2; break;
case lsrawor:
  opstack[sp-2]=(unsigned int)(memw[opstack[sp-2]*2+opstack[sp-1]]>>=(word)opstack[sp]); sp-=2; break;
case lslawor:
  opstack[sp-2]=(unsigned int)(memw[opstack[sp-2]*2+opstack[sp-1]]<<=(word)opstack[sp]); sp-=2; break;
case lcpawor:
  if ((unsigned)opstack[mem[id+_Param]]<256) {
    memw[opstack[sp-1]*2+opstack[sp]]=(word)opstack[mem[id+_Param]];
  } else {
    if (opstack[mem[id+_Param]]<imem_max+258*4)
      memw[opstack[sp-1]*2+opstack[sp]]=memw[opstack[mem[id+_Param]]*2];
    else
      memw[opstack[sp-1]*2+opstack[sp]]=(word)opstack[mem[id+_Param]];
  } sp-=2; mem[id+_Param]++; break;

case lnul:
  #ifdef DEBUG
    if (!opstack[sp]) {
      v_function=-2; e(165);
      if (call_to_debug) { process_stoped=id; return; }
    }
  #endif
  break;

//═════════════════════════════════════════════════════════════════════════════
//  Unused cases (up to 255)
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
case 250: case 251: case 252: case 253: case 254: case 255: break;

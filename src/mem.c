#ifdef __DOS__

//───────────────────────────────────────────────────────────────────────────
//───────────────────────────────────────────────────────────────────────────

void *AllocateDOSMem (uint Size)
{
union REGS r;

	//───────────────────────────────────────────────────────────────────────────
	// Allocate Size / 16 Paragraphs
	// WARNING: In the PMODEW.DOC document the parameters
	// of function 100h are specified incorrectly.
	// To call a real mode interrupt, we must call
	// interrupt 0x31
	//───────────────────────────────────────────────────────────────────────────
	
	SetMem (&r, sizeof (r), 0);

	r.x.eax = 0x0100;				// DOS Mem. Block Allocate function
	r.x.ebx = (Size + 15) >> 4;		// Paragraphs to allocate
	int386 (0x31, &r, &r);

	// Check for error

	if (r.x.cflag)
		return ((uint) _NULL);

	// Return a pointer to the allocated memory area, but first we must
	// convert the real mode address to protected mode, to do so
	// we multiply by 16
	
	return (void *) ((r.x.eax & 0xFFFF) << 4);
}

#endif

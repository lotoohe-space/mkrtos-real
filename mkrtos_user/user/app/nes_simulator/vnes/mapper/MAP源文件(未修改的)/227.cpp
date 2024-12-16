
/////////////////////////////////////////////////////////////////////
// Mapper 227
void NES_mapper227::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,0,1);
}

void NES_mapper227::MemoryWrite(uint32 addr, uint8 data)
{
  uint8 prg_bank = ((addr & 0x0100) >> 4) | ((addr & 0x0078) >> 3);

  if(addr & 0x0001)
  {
    set_CPU_bank4(prg_bank*4+0);
    set_CPU_bank5(prg_bank*4+1);
    set_CPU_bank6(prg_bank*4+2);
    set_CPU_bank7(prg_bank*4+3);
  }
  else
  {
    if(addr & 0x0004)
    {
      set_CPU_bank4(prg_bank*4+2);
      set_CPU_bank5(prg_bank*4+3);
      set_CPU_bank6(prg_bank*4+2);
      set_CPU_bank7(prg_bank*4+3);
    }
    else
    {
      set_CPU_bank4(prg_bank*4+0);
      set_CPU_bank5(prg_bank*4+1);
      set_CPU_bank6(prg_bank*4+0);
      set_CPU_bank7(prg_bank*4+1);
    }
  }
  if(!(addr & 0x0080))
  {
    if(addr & 0x0200)
    {
      set_CPU_bank6((prg_bank & 0x1C)*4+14);
      set_CPU_bank7((prg_bank & 0x1C)*4+15);
    }
    else
    {
      set_CPU_bank6((prg_bank & 0x1C)*4+0);
      set_CPU_bank7((prg_bank & 0x1C)*4+1);
    }
  }

  if(addr & 0x0002)
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }
  else
  {
    set_mirroring(NES_PPU::MIRROR_VERT);
  }
}
/////////////////////////////////////////////////////////////////////


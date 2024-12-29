
/////////////////////////////////////////////////////////////////////
// Mapper 50
class NES_mapper50 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper50(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper50() {}

  void  Reset();
  void  MemoryWriteLow(uint32 addr, uint8 data);
  void  HSync(uint32 scanline);

protected:
  uint8 irq_enabled;

private:
};
/////////////////////////////////////////////////////////////////////


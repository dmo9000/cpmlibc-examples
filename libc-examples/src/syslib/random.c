static unsigned long int next = 1;

int random(void) // RAND_MAX assumed to be 32767
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

void srandom(unsigned int seed)
{
    next = seed;
}

/*
uint32_t seed = 7;  // 100% random seed value

static uint32_t random()
{
  seed ^= seed << 13;
  seed ^= seed >> 17;
  seed ^= seed << 5;
  return seed;
}
*/

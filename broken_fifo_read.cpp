int MezzTesterBoard::ReadFIFO(HitReadout_s * HitReadout)
{
  unsigned int readbuf[RFIFO_DEPTH];
  int readsize = 0;
  // temporary error flag
  int errortemp = 0;
  // cannot get stuck reading empty fifo or program will freeze
  if (FIFOFlags() == FIFO_EMPTY)
    {
      //printf("ERROR: There is nothing to read out...??....\n");
      readsize = NO_READOUT;
      return NO_READOUT;
    }
  printf("\n----------------Readout-----------------\n");
  printf("\t#\tASCII\t\tHEX\n");
  char buffer[20];
  serial.Writeln(" ");
  // read command
  serial.Writeln("tr", false);
  for (readsize = 0; readsize<RFIFO_DEPTH; )
    {
      serial.Readln(buffer, 20);
      readbuf[readsize] = (unsigned int)strtoul(buffer, NULL, 16);
      // check for error word, if it exists then save to temporary error flag
      // and do not increment the write pointer
      // if ((readbuf[readsize] & 0x60000000) == 0x60000000)
      // 	{
      // 	  errortemp |= readbuf[readsize];
      // 	  printf("\t%-d\t%s\t%08X\n", readsize, buffer, readbuf[readsize]);
      // 	  continue;
      // 	}
      printf("\t%-d\t%s\t%08X\n", readsize, buffer, readbuf[readsize]);
      if ((readbuf[readsize] & 0xC0000000) == 0xC0000000)
	break;
      readsize++;
    }
  // check that packet began with header
  if ((readbuf[0] & 0xA0000000) != 0xA0000000)
    printf("ERROR: packet did not begin with header.\n");

  // check number of words written to number received (including header)
  int expected_read = (readbuf[readsize] & 0x000000FFF);
  // if hard error flag recieved, subtract from expected words
  // if ((errortemp & 0x00003E00) != 0)
  //   expected_read--;
  // // if temporal error flag recieved
  // if ((errortemp & 0x000001FF) != 0)
  //   expected_read--;
  // must account for the trailer because quantity starts at 1, not 0
  readsize++;
  if (expected_read != readsize)
    {
      printf("ERROR: fifo read size mismatch error, expected %d but read %d\n", 
	     expected_read, readsize);
      for (int i=0; i<readsize; i++)
	printf("\t%d\t%08X\n", i, readbuf[i]);
      if (errortemp != 0)
	printf("\tE\t%08X\n", errortemp);
      // allow time for user interaction
      sleep(20);
    }
  
  // check that packet ended with trailer

  if ((readbuf[readsize-1] & 0xC0000000) != 0xC0000000)
    printf("ERROR: packet did not end with trailer\n");

  if (readsize == RFIFO_DEPTH)
    printf("ERROR: readsize > RFIFO_DEPTH,\treadsize:%d\tRFIFO_DEPTH:%d\n", 
	   readsize, RFIFO_DEPTH);

  // if ((readsize == RFIFO_DEPTH) && ((readbuf[readsize-1] & 0xC0000000) != 0xC0000000))
  //   printf("ERROR: packet did not end with trailer\n");
  
  if ((readbuf[readsize-1] & 0x00FFF000) != (readbuf[0] & 0x00FFF000))
    printf("ERROR: header and trailer event ID do not match\n");

  //==============================
  // Parsing
  //==============================

  // must have read at least a header and trailer
  // this code should never execute 
  if (readsize < 2)
    {
      printf("ERROR: readsize= %d\n", readsize);
      return READSIZE_ERROR;
    }

  // if we had hits and all channels were disabled, then produce error
  if (EnabledChannel == ALL_OFF)
    {
      printf("ERROR: got hits with all channels on TDC disabled\n");
      // return READSIZE_ERROR;
    }

  // parse hit data
  int i;
  for (i=1; i < readsize-1;)
    {
      // make sure we are only reading measurement data
      if (((readbuf[i] & 0x30000000) != 0x30000000) &&
	  ((readbuf[i] & 0x60000000) != 0x60000000))
	{
	  printf("ERROR: word is not error or measurement data: %08X\n", readbuf[i]);
	  continue;
	}
      else if ((readbuf[i] & 0x60000000) == 0x60000000)
	{
	  errortemp |= readbuf[i];
	  continue;
	}
      // record the channel
      HitReadout->hits[i-1].channel = (readbuf[i] & 0x00F80000) >> 19;
      // make sure it was the enabled channel
      if (HitReadout->hits[i-1].channel != EnabledChannel)
	printf("ERROR: Hit on channel %d, but only channel %d enabled\n",
	       HitReadout->hits[i-1].channel, EnabledChannel);
      HitReadout->hits[i-1].edge = (readbuf[i] & 0x00040000) >> 18;
      HitReadout->hits[i-1].error = (readbuf[i] & 0x00020000) >> 17;
      HitReadout->hits[i-1].coarseTime = (readbuf[i] & 0x0001FFE0) >> 5;
      HitReadout->hits[i-1].fineTime = (readbuf[i] & 0x0000001F);
      // calcualte trigger matched time in ns
      HitReadout->hits[i-1].hitTime = (HitReadout->hits[i-1].coarseTime*32 +
				       HitReadout->hits[i-1].fineTime)*.78125;
    }


  // save error flags
  HitReadout->errorflags = errortemp & 0x00003FFF;
  // write error flags if present
  if (HitReadout->errorflags != 0)
    printf("ERROR: error flags: %04X\n", HitReadout->errorflags);
  // number of hits is packet size - errorflags - header/trailer;
  HitReadout->numHits = i-1;
  // mask and save event ID
  HitReadout->eventID = (readbuf[0] & 0x00FFF000) >> 12;
  // mask and save bunch count ID
  HitReadout->bunchID = (readbuf[0] & 0x00000FFF);

  // if there were no hits
  if (HitReadout->numHits == 2)
    {
      HitReadout->numHits = 0;
      // printf("No hits\n");
      return NO_HITS;
    }


  return 1;

}

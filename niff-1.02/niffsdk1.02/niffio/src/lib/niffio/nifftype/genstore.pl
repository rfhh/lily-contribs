#!/usr/bin/perl

require "loadobs.pl";
require "getopt.pl";

&Getopt('');

&loadobs;
@SortedKeys = sort keys(%ObjectTypes);


&doform;
&dolists;
&dorawchunks;
&docookedchunks;
&dorawtags;
&docookedcomptags;
&docookedprimtags;

sub doform
{
print "/***************************\n";
print " * Form\n";
print " ****************************/\n";

# Form Store routine
#
foreach $ObjectName (@SortedKeys)
{
	local($header);

	($ObjectTypes{$ObjectName} ne 'form') && next;
	
	if ($opt_h)
	{
		print <<"END";
extern RIFFIOSuccess
NIFFIOStart$ObjectName(void);
extern RIFFIOSuccess
NIFFIOEnd$ObjectName(void);

END
    }
	else
    {
		print <<"END";

RIFFIOSuccess
NIFFIOStart$ObjectName(void)
{
    
	RIFFIOChunk chunkForm;

	assert(!NIFFIOStorageIsListPending());

	chunkForm.fccId = RIFFIO_FOURCC_RIFX;
	chunkForm.fccType = niffform$ObjectName;

	if (! NIFFIOStorageListStart(&chunkForm))
		return RIFFIO_FAIL;

	return RIFFIO_OK;

}

RIFFIOSuccess
NIFFIOEnd$ObjectName(void)$header
{
	assert (NIFFIOStoragePendingList()->fccType ==
			niffform$ObjectName);
	
			
	if (!NIFFIOStorageListEnd())
		return RIFFIO_FAIL;
	
    assert(! NIFFIOStorageIsListPending()); /* no more lists pending */
	return RIFFIO_OK;
}

END
    } # else not header
} # foreach form 

} # doform


# List Store routines
#
sub dolists
{
print "/***************************\n";
print " * Lists\n";
print " ****************************/\n";

foreach $ObjectName (@SortedKeys)
{
	local($header);

	($ObjectTypes{$ObjectName} ne 'list') && next;
	
	if ($opt_h)
	{
		print <<"END";
extern RIFFIOSuccess
NIFFIOStart$ObjectName(void);
extern RIFFIOSuccess
NIFFIOEnd$ObjectName(void);

END
    }
	else
    {
		print <<"END";

RIFFIOSuccess
NIFFIOStart$ObjectName(void)
{
    
	RIFFIOChunk chunkList;

	chunkList.fccId = RIFFIO_FOURCC_LIST;
	chunkList.fccType = nifflist$ObjectName;

	if (! NIFFIOStorageListStart(&chunkList))
		return RIFFIO_FAIL;

	return RIFFIO_OK;

}

RIFFIOSuccess
NIFFIOEnd$ObjectName(void)$header
{
	assert (NIFFIOStoragePendingList()->fccType ==
			nifflist$ObjectName);
	
	if (!NIFFIOStorageListEnd())
		return RIFFIO_FAIL;
	
	return RIFFIO_OK;
}

END
    } # else not header
} # foreach list 

} # dolists

sub dorawchunks
{
print "/***************************\n";
print " * Raw chunks\n";
print " ****************************/\n";

###############
# Raw Chunks
###############
foreach $ObjectName (@SortedKeys)
{
	local($header);

	if (! (($ObjectTypes{$ObjectName} eq 'chunk') 
      && ! $ObjectCooked{$ObjectName}))
   { next; }

	
	if ($opt_h)
	{
		print <<"END";
extern RIFFIOSuccess
NIFFIOchunk$ObjectName(void);

END
    }
	else
    {
		print <<"END";

RIFFIOSuccess
NIFFIOchunk$ObjectName(void)
{
    
	RIFFIOChunk chunk;


	chunk.fccId = niffckid$ObjectName;

	if (! NIFFIOStorageChunkStart(&chunk))
		return RIFFIO_FAIL;

	return RIFFIO_OK;

}

END
    } # else not header
} # foreach chunk

} # dorawchunks


#################
## Cooked chunks
#################
sub docookedchunks
{
print "/***************************\n";
print " * Cooked chunks\n";
print " ****************************/\n";

foreach $ObjectName (@SortedKeys)
{
	if (! (($ObjectTypes{$ObjectName} eq 'chunk') 
      && $ObjectCooked{$ObjectName}))
   { next; }

    local (@members) = &splitmembers($ObjectMembers{$ObjectName});
    local (@arglist) = "\n    " . join (",\n    ", @members);
	if ($opt_h)
    {
	print <<"END";
extern
RIFFIOSuccess
NIFFIOchunk$ObjectName(@arglist);

END
    }
    else
    {
#	print "    ", join(",\n    ", @members), "\n";
	print <<"END";
RIFFIOSuccess
NIFFIOchunk$ObjectName(@arglist)
{
    
    RIFFIOChunk chunk;
    niff$ObjectName $ObjectName;

    /* Initialize the chunk's structure */
END
   foreach $member (@members)
   {
	   ($type, $membername, $arraylen) = &splitdeclarations($member);
	   if (($type eq 'CHAR') && $arraylen)
       {
		 print "    strncpy((char *) $ObjectName.$membername, \n";
		 print "            (char *) $membername, \n";
         print "            $arraylen);\n";
       }
       else
       {
          print "    $ObjectName.$membername = $membername;\n";
       }
   }
   print <<"END";

    /* Write the chunk header */
    chunk.fccId = niffckid$ObjectName;

    if (! NIFFIOStorageChunkStart(&chunk))
        return RIFFIO_FAIL;

    /* Write the chunk structure */
    if (! NIFFIOWriteniff$ObjectName(NIFFIOStorageGetFile(), 
                                     &$ObjectName))
        return RIFFIO_FAIL;

    return RIFFIO_OK;

}

END
    } # else not header
} # foreach cooked chunk
		
} # docookedchunks

###############
# Raw Tags
###############
sub dorawtags
{
print "/***************************\n";
print " * Raw tags\n";
print " ****************************/\n";

foreach $ObjectName (@SortedKeys)
{
	local($header);

	if (! (($ObjectTypes{$ObjectName} eq 'tag') 
      && ! $ObjectCooked{$ObjectName}))
   { next; }

	
	if ($opt_h)
	{
		print <<"END";
extern RIFFIOSuccess
NIFFIOtag$ObjectName(void);

END
    }
	else
    {
		print <<"END";

RIFFIOSuccess
NIFFIOtag$ObjectName(void)
{
    
	NIFFIOTag tag;


	tag.tagid = nifftag$ObjectName;

	if (! NIFFIOStorageTagStart(&tag))
		return RIFFIO_FAIL;

	return RIFFIO_OK;

}

END
    } # else not header
} # foreach tag

} # dorawtags



#################
## Cooked composite tags
#################
sub docookedcomptags
{
print "/***************************\n";
print " * Cooked composite tags\n";
print " ****************************/\n";

foreach $ObjectName (@SortedKeys)
{
	if (! (($ObjectTypes{$ObjectName} eq 'tag') 
      && $ObjectCooked{$ObjectName}
      && $ObjectMembers{$ObjectName}))
   { next; }

    local (@members) = &splitmembers($ObjectMembers{$ObjectName});
    local (@arglist) = "\n    " . join (",\n    ", @members);
	if ($opt_h)
    {
	print <<"END";
extern
RIFFIOSuccess
NIFFIOtag$ObjectName(@arglist);

END
    }
    else
    {
#	print "    ", join(",\n    ", @members), "\n";
	print <<"END";
RIFFIOSuccess
NIFFIOtag$ObjectName(@arglist)
{
    
    NIFFIOTag tag;
    niff$ObjectName $ObjectName;

    /* Initialize the tag's structure */
END
   foreach $member (@members)
   {
	   ($type, $membername, $arraylen) = &splitdeclarations($member);
	   if (($type eq 'CHAR') && $arraylen)
       {
		 print "    strncpy((char *) $ObjectName.$membername, \n";
		 print "            (char *) $membername, \n";
         print "            $arraylen);\n";
       }
       else
       {
          print "    $ObjectName.$membername = $membername;\n";
       }
   }
   print <<"END";

    /* Write the tag header */
    tag.tagid = nifftag$ObjectName;

    if (! NIFFIOStorageTagStart(&tag))
        return RIFFIO_FAIL;

    /* Write the tag structure */
    if (! NIFFIOWriteniff$ObjectName(NIFFIOStorageGetFile(), 
                                     &$ObjectName))
        return RIFFIO_FAIL;

    return RIFFIO_OK;

}

END
    } # else not header
} # foreach cooked tag
		
} # docookedcomptags


########################
## Cooked primitive tags
########################
sub docookedprimtags
{
print "/***************************\n";
print " * Cooked primitive tags\n";
print " ****************************/\n";

foreach $ObjectName (@SortedKeys)
{
	if (! (($ObjectTypes{$ObjectName} eq 'tag') 
      && $ObjectCooked{$ObjectName}
      && $ObjectPrimitive{$ObjectName}))
   { next; }

	local($arglist) = "niff$ObjectName $ObjectName";
	if ($opt_h)
    {
	print <<"END";
extern
RIFFIOSuccess
NIFFIOtag$ObjectName($arglist);

END
    }
    else
    {
	print <<"END";
RIFFIOSuccess
NIFFIOtag$ObjectName($arglist)
{
    
    NIFFIOTag tag;

    /* Write the tag header */
    tag.tagid = nifftag$ObjectName;

    if (! NIFFIOStorageTagStart(&tag))
        return RIFFIO_FAIL;

    /* Write the tag structure */
    if (! NIFFIOWriteniff$ObjectName(NIFFIOStorageGetFile(), 
                                     &$ObjectName))
        return RIFFIO_FAIL;

    return RIFFIO_OK;

}

END
    } # else not header
} # foreach cooked tag
		
} # docookedprimtags

#!/usr/bin/perl

#
# 
# loadobs
# ===========
# Read niff.h (provided on the command line, or as STDIN) and
# create global asssociative arrays that store the types of 
# NIFF objects (lists,chunks,tags).
#
# %ObjectTypes - keyed on an object's name (eg. Notehead)
#    values may be: 'form', 'list', 'chunk', 'tag'.
#
# %ObjectCooked - keyed on an object's name.
#    Boolean values, true if an object has an associated structure.
#
# %ObjectMembers - keyed on an object's name.
#    Values are strings with semicolon-delimited fields
#    Each field is a C-style declaration of the members of 
#    an object's structure.  For example 
#  
#   "CHAR NIFFVersion [8]; BYTE programType; BYTE standardUnits;
#    SHORT absoluteUnits; SHORT midiClocksPerQuarter;"
#
# %ObjectPrimitive - keyed on an object's name
#    Hold the primitive type of a cooked object.
#    Used when the object has a primitive type instead of %ObjectMembers
#      
# %ObjectLength - keyed on an object's name
#    The value stored in the chunk length table
#

sub loadobs
{
	# Read each line of niff.h
    # Try to recognize each line as 
	#   * Defining a list, chunk, or tag
    #   * Defining the structure of a chunk or tag
	while (<>)
	{
		# 
		local($ObjectName); # The name of each object (Eg. "Notehead")
	    local($line) = $_;

	    ########################
	    # Look for chunk lengths
        ########################
    	if (/^#define\s+niffcklen(\w+)\s+(\d+)/)
	    {
	       $ObjectName = $1;
		   local($ObjectLength) = $2;

		   $ObjectLength{$ObjectName} = $ObjectLength;
        }    
		
		# Look for a form
        # Match lines like "#define niffformNiff MAKEFOURCC(...)"
		if (/^#define\s+niffform(\w+)/)
		{
			$ObjectName = $1; # 
			$ObjectTypes{$ObjectName} = 'form';
		}      	

		# Look for a list
        # Match lines like "#define nifflistPage MAKEFOURCC(...)"
		if (/^#define\s+nifflist(\w+)/)
		{
			$ObjectName = $1; # 
			$ObjectTypes{$ObjectName} = 'list';
		}      	
   

		# Look for a chunk
		# Match lines like "#define niffckid MAKEFOURCC(...)"
		if (/^#define\s+niffckid(\w+)/)
		{
			$ObjectName = $1;
		    if ($ObjectName ne 'UserDefined') 
			{ 
				$ObjectTypes{$ObjectName} = 'chunk';
			}
		}      	
   
		# Look for a tag
		# Match lines like "enum { nifftagPartID };
		if (/enum\s*{\s*nifftag(\w+)/)
	    {
		    $ObjectName = $1;
	 	    if ($ObjectName ne 'UserDefined')
		    {
		       $ObjectTypes{$ObjectName} = 'tag';
		    }
		}

	    # Look for primitive cooked objects
        # Match lines that look like "typedef BYTE niffAltEnding"

		if (/^typedef\s+(\w+)\s+niff(\w+)/)
		{
			$ObjectDecl = $1;
			$ObjectName = $2;
			
			if (!($ObjectDecl eq 'struct'))
			{
				$ObjectPrimitive{$ObjectName} = $ObjectDecl;
				$ObjectCooked{$ObjectName} = 1;
			}
		}

	    # Look for structures (cooked objects)
	    # Match blocks of lines that start "typedef struct niffNiffInfo"
	    if (/^typedef\s+struct\s+niff(\w+)/)
	    {
	        $ObjectName = $1;
		    $ObjectCooked{$ObjectName} = 1;

			while (($_ = <>) !~ /niff$ObjectName\s*;/)
			{
				chop; # chop newline
				
				# Does this line look like a field definition?
				if ($_ =~ /(\w+)\s+(\w+)\s*(\[.+\])?\s*;/ )
				{
					# Yes, append the field definition to our
					# ObjectMembers entry
					$type = $1;
					$member = $2;
					$array = $3;
					$ObjectMembers{$ObjectName} .= "$type $member $array ;";
				}


		}


	}
}

} # sub LoadObjects


#
# splitmembers
# ============
# Given a semi-colon delimited string of member definitions
# return an array of member definitions

sub splitmembers
{
	local($members) = @_;

	$members =~ s/\s*;\s*$//;  # get rid of last semi-colon
	split(/;/, $members);      # return array of member definitions
}

#
# splitdefinition
# ===============
# Given a C-style declaration, return a list consisting of
# (type, member, array)
# array may be null
#
sub splitdeclarations
{
    local($declaration) = @_;
	local($type);
	local($member);
	local($arraylen);

    if ($declaration =~ /(\w+)\s+(\w+)\s*(\[\s*(\d+)\s*\])?/)
    {
		$type =   $1;
		$member = $2;
		$arraylen = $4;
    }

   ($type, $member, $arraylen); 
}

1;  # Return OK




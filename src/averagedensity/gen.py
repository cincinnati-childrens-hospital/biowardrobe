#!/usr/bin/python

import os
import sys

print """@HD	VN:1.0	SO:unsorted											
@SQ	SN:chr10	LN:135534747											
@SQ	SN:chr11	LN:135006516											
@SQ	SN:chr12	LN:133851895											
@SQ	SN:chr13	LN:115169878											
@SQ	SN:chr14	LN:107349540											
@SQ	SN:chr15	LN:102531392											
@SQ	SN:chr16	LN:90354753											
@SQ	SN:chr17	LN:81195210											
@SQ	SN:chr18	LN:78077248											
@SQ	SN:chr19	LN:59128983											
@SQ	SN:chr1	LN:249250621											
@SQ	SN:chr20	LN:63025520											
@SQ	SN:chr21	LN:48129895											
@SQ	SN:chr22	LN:51304566											
@SQ	SN:chr2	LN:243199373											
@SQ	SN:chr3	LN:198022430											
@SQ	SN:chr4	LN:191154276											
@SQ	SN:chr5	LN:180915260											
@SQ	SN:chr6	LN:171115067											
@SQ	SN:chr7	LN:159138663											
@SQ	SN:chr8	LN:146364022											
@SQ	SN:chr9	LN:141213431											
@SQ	SN:chrM	LN:16571											
@SQ	SN:chrX	LN:155270560											
@SQ	SN:chrY	LN:59373566											"""

#print """HWI-11	0	chr1	1	255	25M	*	0	0	CAGTGGAATGTTCTTTTGGATGCCC	[[[H[[[[U[[[[[PUU[U[[[[[[	XA:i:0	MD:Z:25	NM:i:0"""

for i in range(1,249250621):
 if i%50==0:
  print """HWI-%d	0	chr1	%d	255	25M	*	0	0	CAGTGGAATGTTCTTTTGGATGCCC	[[[H[[[[U[[[[[PUU[U[[[[[[	XA:i:0	MD:Z:25	NM:i:0"""%(i,i)
 
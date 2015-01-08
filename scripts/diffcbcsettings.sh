#/bin/bash

if [ $# -ne 2 ];then
	echo "${0} [file1] [file2]"
	exit 0;
fi

join -o 1.1,1.5,2.5 ${1} ${2} | gawk '/^[^*]/{n1=strtonum($2); n2=strtonum($3);d=n1-n2;printf("%30s 0x%02X 0x%02X %3d\n", $1, n1, n2, d)}/Channel[0-9]/{sum+=d;sum2+=(d*d);}END{m=sum/254.;printf("Offset diff=%5.2f\+/-%6.3f\n", m, sqrt((sum2/254.-m*m)))}' 


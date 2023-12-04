#!/bin/sh

exedir=`dirname $0`/../etc

echo "" >&2
echo "    Probing for system information..." >&2
echo "" >&2

echo ""                    >  ./SYS_INFO.TXT
echo "Platform Info:"      >> ./SYS_INFO.TXT
echo ""                    >> ./SYS_INFO.TXT
$exedir/platform_info.sh   >> ./SYS_INFO.TXT
$exedir/free_diskspace.sh  >> ./SYS_INFO.TXT
echo ""                    >> ./SYS_INFO.TXT

echo ""                    >> ./SYS_INFO.TXT
echo "Network Info:"       >> ./SYS_INFO.TXT
echo ""                    >> ./SYS_INFO.TXT
$exedir/net_info.sh        >> ./SYS_INFO.TXT
$exedir/mcast_route.sh     >> ./SYS_INFO.TXT
echo ""                    >> ./SYS_INFO.TXT

echo ""                    >> ./SYS_INFO.TXT
echo "Compiler Info:"      >> ./SYS_INFO.TXT
echo ""                    >> ./SYS_INFO.TXT
$exedir/compiler_info.sh   >> ./SYS_INFO.TXT
echo ""                    >> ./SYS_INFO.TXT

echo "    The system information has been written to a file named SYS_INFO.TXT." >&2
echo "    Please review this information, and note any discrepancies between" >&2
echo "    what was found and what you think should have been found." >&2
echo "" >&2
echo "    You may be asked to email the contents of SYS_INFO.TXT to RTI Support" >&2
echo "    to assist in configuring or diagnosing your RTI installation." >&2
echo "" >&2


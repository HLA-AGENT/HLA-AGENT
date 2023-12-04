#! /usr/local/bin/zsh -f
#
# assume RTI_HOME and RTI_PLATFORM are set.
# and that cwd is $RTI_HOME/config.
#

cd config

echo "Customizing configuration files for your system..."

sed -e "s:_INSTALLDIRECTORY:${RTI_HOME}:" -e "s:_RTIBUILDTYPE:${RTI_BUILD_TYPE}:" rtienv.csh.ori > rtienv.csh
chmod 644 rtienv.csh
rm -f rtienv.csh.ori

sed -e "s:_INSTALLDIRECTORY:${RTI_HOME}:" -e "s:_RTIBUILDTYPE:${RTI_BUILD_TYPE}:"  rtienv.sh.ori > rtienv.sh
chmod 755 rtienv.sh
rm -f rtienv.sh.ori

echo "Customizing sample RTI.rid for your system..."

cd ../doc

sed -e "s:INSTALLDIRECTORY:${RTI_HOME}/${RTI_BUILD_TYPE}:" Sample-RTI.rid.ori > Sample-RTI.rid
chmod 644 Sample-RTI.rid
rm -f Sample-RTI.rid.ori

echo "Done."


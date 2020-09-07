#!/bin/bash - 
#Post build for SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256
# arg1 is the build directory
# arg2 is the elf file path+name
# arg3 is the bin file path+name
# arg4 is the version
# arg5 when present forces "bigelf" generation
projectdir=$1
FileName=${3##*/}
execname=${FileName%.*}
elf=$2
bin=$3
version=$4

SBSFUBootLoader=${0%/*}"/../.." 
# The default installation path of the Cube Programmer tool is: 
# If you installed it in another location, please update the %programmertool% variable below accordingly.
uname -o | grep -i -e windows -e mingw

if [ $? == 0 ]; then
  #fix programmer tool standard install 
  export PATH="/c/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin":$PATH
  programmertool="STM32_Programmer_CLI"
#else
#programmertool access path for other os
fi
userAppBinary=$projectdir"/../Binary"

sfu=$userAppBinary"/"$execname".sfu"
sfb=$userAppBinary"/"$execname".sfb"
sign=$userAppBinary"/"%execname".sign"
headerbin=$userAppBinary"/"$execname"sfuh.bin"
bigbinary=$userAppBinary"/SBSFU_"$execname".bin"
elfbackup=$userAppBinary"/SBSFU_"$execname".elf"

iv=$SBSFUBootLoader"/1_Image_SECoreBin/Binary/iv.bin"
oemkey=$SBSFUBootLoader"/1_Image_SECoreBin/Binary/OEM_KEY_COMPANY1_key_AES_CBC.bin"
ecckey=$SBSFUBootLoader"/1_Image_SECoreBin/Binary/ECCKEY.txt"
sbsfuelf=$SBSFUBootLoader"/1_Image_SBSFU/SW4STM32/STM32F413H_DISCOVERY_1_Image_SBSFU/Debug/SBSFU.elf"

current_directory=`pwd`
cd $1/../../../../../../Middlewares/ST/STM32_Secure_Engine/Utilities/KeysAndImages
basedir=`pwd`
cd $current_directory
# test if window executeable useable
prepareimage=$basedir"/win/prepareimage/prepareimage.exe"
uname | grep -i -e windows -e mingw

if [ $? == 0 ] && [   -e "$prepareimage" ]; then
  echo "prepareimage with windows executeable"
  cmd=""
else
  # line for python
  echo "prepareimage with python script"
  prepareimage=$basedir/prepareimage.py
  cmd="python"
fi

echo "$cmd $prepareimage" >> $1/output.txt
# Make sure we have a Binary sub-folder in UserApp folder
if [ ! -e $userAppBinary ]; then
mkdir $userAppBinary
fi

#PostBuild is fired if elf has been regenerated from last run, so elf is different from backup elf
if [ ! -e $elfbackup ]; then 
  diff $elfbackup $elf > /dev/null 
  ret=$?
  if [ $ret != 0 ]; then
    echo "elf has been modified, processing required"
    process=1
  else
    process=0
  fi
fi

if [ $process == 1 ]; then
  command=$cmd" "$prepareimage" enc -k "$oemkey" -i "$iv" "$bin" "$sfu
  $command > "$projectdir"/output.txt
  ret=$?
  if [ $ret == 0 ]; then
    command=$cmd" "$prepareimage" sha256 "$bin" "$sign
    $command >> $projectdir"/output.txt"
    ret=$?
    if [ $ret == 0 ]; then 
      command=$cmd" "$prepareimage" pack -k "$ecckey" -r 4 -v "$version" -i "$iv" -f "$sfu" -t "$sign" "$sfb" -o 512"
      $command >> $projectdir"/output.txt"
      ret=$?
      if [ $ret == 0 ]; then
        command=$cmd" "$prepareimage" header -k  "$ecckey" -r 4 -v "$version"  -i "$iv" -f "$sfu" -t "$sign" -o 512 "$headerbin
        $command >> $projectdir"/output.txt"
        ret=$?
        if [ $ret == 0 ]; then
          command=$cmd" "$prepareimage" merge -v 0 -e 1 -i "$headerbin" -s "$sbsfuelf" -u "$elf" "$bigbinary
          $command >> $projectdir"/output.txt"
          ret=$?
          if [ $ret == 0 ] && [ $# == 5 ]; then
            echo "Generating the global elf file SBSFU and userApp"
            echo "Generating the global elf file SBSFU and userApp" >> $projectdir"/output.txt"
            command=$programmertool" -ms "$elf" "$headerbin" "$sbsfuelf
            $command >> $projectdir"/output.txt"
            ret=$?
            if [ $ret == 0 ]; then 
              command="cp "$elf" "$elfbackup  
              $command >> $projectdir"/output.txt"
              ret=$?
            fi
          fi
        fi
      fi
    fi
  fi
fi

if [ $ret == 0 ]; then
  rm $sign
  rm $sfu
  rm $headerbin
  exit 0
else 
  echo "$command : failed" >> $projectdir"/output.txt"
  if [ -e  "$elf" ]; then
    rm  $elf
  fi
  if [ -e "$elfbackup" ]; then 
    rm  $elfbackup
  fi
  echo $command : failed
  read -n 1 -s
  exit 1
fi

# make file for generating a binary for the esp32 under the arduino eco-system
###############################################################################

# compiler args

# project name
project_name		= meshy-tron-firmware

# fully qualified board name for the bluepill, use "make list-boards" to view available boards
hardware_fqbn 		= esp32:esp32:esp32s3

# set "true" to enable verbose 
enable_verbose 		= false

# default binary upload method = stlink swd, for USB or UART the stm32 will need to reboot onto the bootloader before running make
default_bin-upload	= open-ocd-st-link-v2

# build dir full path, default is at current work directory / build
# build_dir			= full-path-to-build-directory

# additional boards URL - ESP32
additional_urls = "https://raw.githubusercontent.com/espressif/arduino-esp32/master/package.json"

# chip name, refer to esputil source
chip_name = ESP32-S3-BETA3

# upload baudrate
upload_baud = 1500000

# upload port
upload_port = /dev/ttyACM0

# flash param for flash type, size and frequency refer to esputil readme for more info
flash_param = 0x24f

###############################################################################

# make params
acli_workspace_name = .workspace-ard-cli
work_dir    		:= $(PWD)
acli_path			:= ${work_dir}/${acli_workspace_name}
esputil_path		:= ${acli_path}/tools/esputil
esputil_bin_path	:= ${esputil_path}/esputil
acli_config 		:= ${acli_path}/config.json
host_epoch 			:= $(shell date +%s)
host_time			:= $(shell date)

acli_staging	:= ${acli_path}/staging
acli_ard		:= ${acli_path}/Arduino


# verbose flag
vf      	 			=
ifeq "${enable_verbose}" "true"
vf          			=-v
endif

# default build dir
build_dir  		?= ${work_dir}/build

# bin file name & path
bin_path        = ${build_dir}/${project_name}.ino.bin

SRCS := ${project_name}.ino ${project_name}.h $(wildcard src/*.c) $(wildcard src/*.h)

###############################################################################

# make targets

# binary
compile: dev
	echo "build evoked at ${host_time} for the ${hardware_fqbn}"
	echo "output files will be written to ${build_dir}"
	arduino-cli compile -b ${hardware_fqbn} ${vf} --config-file ${acli_config} --output-dir ${build_dir}


# target to clean the build dir
clean:
	@if [ -d ${build_dir} ]; \
	then \
		echo removing ${build_dir}; \
		rm -rf ${build_dir}; \
	fi


# target to init the build env this target tracks an md file
${acli_config}:	
	$(info creating workspace at ${work_dir}/${acli_workspace_name})
	arduino-cli ${vf} config init --overwrite --dest-file ${acli_config}
	arduino-cli ${vf} config set board_manager.additional_urls ${additional_urls} --config-file ${acli_config}
	arduino-cli ${vf} config set directories.downloads ${acli_staging} --config-file ${acli_config}
	arduino-cli ${vf} config set directories.data ${acli_path} --config-file ${acli_config}
	arduino-cli ${vf} config set directories.user ${acli_ard} --config-file ${acli_config}
	arduino-cli ${vf} lib update-index --config-file ${acli_config}
	arduino-cli ${vf} core update-index --config-file ${acli_config}
	arduino-cli ${vf} core install esp32:esp32 --config-file ${acli_config}

# target to remove the dev env
dev-remove:
	@if [ -d "${acli_path}" ]; \
	then \
		echo removing dev env at  ${acli_path}; \
		rm -rf ${acli_path}; \
	fi

# target to download and compile esputil, much better than the python crap
esputil-install:
	@if [ ! -e "${esputil_path}" ]; \
	then \
		git clone git@github.com:cnlohr/esputil.git ${esputil_path}; \
		cd ${esputil_path} && make; \
		cd ${work_dir}; \
	fi

# alias for the config file
dev: ${acli_config} esputil-install

# target to install arduino platform libs listed under libindex
lib-install: dev
	@if [ -e "${work_dir}/libindex" ]; \
	then \
		while read -r line; do \
			if [ -n "$$line" ]; \
			then \
   				echo "installing library $$line"; \
				arduino-cli lib install "$$line" --config-file ${acli_config}; \
			fi; \
		done < "${work_dir}/libindex"; \
	fi

# target to remove all libraries listed under libindex
lib-remove: dev
	@if [ -e "${work_dir}/libindex" ]; \
	then \
		while read -r line; do \
			if [ -n "$$line" ]; \
			then \
   				echo "installing library $$line"; \
				arduino-cli lib uninstall "$$line" --config-file ${acli_config}; \
			fi; \
		done < "${work_dir}/libindex"; \
	fi

# target to view all available boards
list-boards:
	arduino-cli board listall --config-file ${acli_config}

# upload app only
upload-app:
	${esputil_bin_path} -chip ${chip_name} -p ${upload_port} -fp ${flash_param} -b ${upload_baud} flash 0x10000 ${build_dir}/${project_name}.ino.bin

# upload bootloader, partition info and app
upload-all: 
	${esputil_bin_path} -chip ${chip_name} -p ${upload_port} -fp ${flash_param} -b ${upload_baud} flash 0x0 ${build_dir}/${project_name}.ino.bootloader.bin 0x8000 ${build_dir}/${project_name}.ino.partitions.bin 0x10000 ${build_dir}/${project_name}.ino.bin

#default upload
upload: upload-app

# target for all
all: compile upload-all

# define the phony targets
.PHONY: all compile clean dev dev-remove lib-install lib-remove list-boards upload upload-app upload-all esputil-install
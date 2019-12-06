Vagrant.configure(2) do |config|
  config.vm.box = "ubuntu/trusty64"
  config.vm.provision "shell", inline: <<-SHELL
		sudo locale-gen UTF-8
		sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test 
		sudo apt-get update
		sudo apt-get -y install gcc-8
		sudo apt-get -y install nasm
		sudo apt-get -y install qemu
		sudo apt-get -y install xorriso
		wget ftp://ftp.gnu.org/gnu/mtools/mtools-4.0.23.tar.gz
		tar xvzf mtools-4.0.23.tar.gz
		cd mtools-4.0.23
		./configure
		make
		sudo make install
		sudo ln -s /usr/bin/gcc-8 /usr/bin/x86_64-elf-gcc
		sudo ln -s /usr/bin/ld /usr/bin/x86_64-elf-ld
		sudo ln -s /usr/bin/as /usr/bin/x86_64-elf-as
		sudo ln -s /usr/bin/nm /usr/bin/x86_64-elf-nm
		sudo ln -s /usr/bin/objdump /usr/bin/x86_64-elf-objdump  
  SHELL
end
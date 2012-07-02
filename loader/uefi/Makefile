all:
	cp -rf * $(UDKPATH)/MyWorkSpace/MdeModulePkg/Application/RoseUefi/
	build -p MdeModulePkg/MdeModulePkg.dsc -a X64 -m MdeModulePkg/Application/RoseUefi/uefi.inf

clean:
	build -p MdeModulePkg/MdeModulePkg.dsc -a X64 -m MdeModulePkg/Application/RoseUefi/uefi.inf clean
all:
	cp * $(UDKPATH)/MyWorkSpace/MdeModulePkg/Application/RoseUefi/
	cd $(UDKPATH)/MyWorkSpace/; \
	build -p MdeModulePkg/MdeModulePkg.dsc -a X64 -m MdeModulePkg/Application/RoseUefi/uefi.inf

clean:
	cd $(UDKPATH)/MyWorkSpace/; \
	build -p MdeModulePkg/MdeModulePkg.dsc -a X64 -m MdeModulePkg/Application/RoseUefi/uefi.inf clean
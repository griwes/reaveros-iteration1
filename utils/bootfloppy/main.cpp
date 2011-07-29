// there is one flaw in this one: it adds 1 to size of each of files written into this filesystem
// if they are rounded to 512 bytes. but making tests to check such cases would make more troubles
// and unclear code than making this one better.

#include <iostream>
#include <fstream>

using namespace std;

void usage()
{
	cout << "ReaverFS bootfloppy creator, version 0.1" << endl << endl;
	cout << "Usage:" << endl;
	cout << "./bootfloppy <output file> <first stage bootloader> <second stage bootloader> <kernel> [<initrd>]" << endl;
	return;
}

void zero(char * table, int size)
{
	for (int i = 0; i < size; i++)
		table[i] = 0;
	
	return;
}

// parameters:
// 1. floppy image file
// 2. first stage bootloader
// 3. second stage bootloader
// 4. kernel
// 5. initial ramdisk image (may be ommited)
int main(int argc, char ** argv)
{
	if (argc < 5 || argc > 6)
	{
		usage();
		return 0;
	}
	
	fstream output(argv[1], fstream::out | fstream::trunc | fstream::binary);
	if (!output.good())
	{
		cout << "Couldn't open output file." << endl;
		return 0;
	}
	
	fstream stage1(argv[2], fstream::in | fstream::binary);
	if (!stage1.good())
	{
		cout << "Couldn't open stage 1 file." << endl;
		return 0;
	}
	
	fstream stage2(argv[3], fstream::in | fstream::binary);
	if (!stage2.good())
	{
		cout << "Couldn't open stage 2 file." << endl;
		return 0;
	}
	
	fstream kernel(argv[4], fstream::in | fstream::binary);
	if (!kernel.good())
	{
		cout << "Couldn't open kernel image file." << endl;
		return 0;
	}
	
	if (argc == 6)
	{
		fstream initrd(argv[5], fstream::in | fstream::binary);
		if (!stage1.good())
		{	
			cout << "Couldn't open initrd file." << endl;
			return 0;
		}
	}
	
	char * buffer = new char[512];
	zero(buffer, 512);

	// it should be 512 bytes long, if it is not - it's not our problem, is it?
	stage1.read(buffer, 512);
	output.write(buffer, 512);
	zero(buffer, 512);
	
	stage1.close();
	
	// now read stage 2 and write it to output file
	// thanks to simple trick, it will be made it rounded to 1 sector
	// (512 bytes). zero (buffer, 512) set's it to 0; when data from
	// file is loaded and there is less than 512 bytes, it will be
	// filled up with 0s
	short counter = 0;
	while (!stage2.eof())
	{
		stage2.read(buffer, 512);
		output.write(buffer, 512);
		zero(buffer, 512);
		counter++;
	}
	stage2.close();
	
	// here the fun part begin. we must write size (in sectors) into
	// stage 1 header. Huh, we could store it in memory and not bother
	// with modifying file, yes? Yes, but it would be less fun than this one.
	// and what is os development all about? ...yes, fun.
	int pos = output.tellp();
	output.seekp(22);
	output.write(reinterpret_cast<char*>(&counter), sizeof(counter));
	output.seekp(pos);

	// now almost the same with kernel image
	short kernelcounter = 0;
	while (!kernel.eof())
	{
		kernel.read(buffer, 512);
		output.write(buffer, 512);
		zero(buffer, 512);
		kernelcounter++;
	}
	kernel.close();
	
	pos = output.tellp();
	output.seekp(512 + 8);
	output.write(reinterpret_cast<char*>(&kernelcounter), sizeof(kernelcounter));
	output.seekp(pos);
	
	short initrdcounter = 0;
	if (argc == 6)
	{
		fstream initrd(argv[5], fstream::in | fstream::binary);
		while (!initrd.eof())
		{
			initrd.read(buffer, 512);
			output.write(buffer, 512);
			zero(buffer, 512);
			initrdcounter++;
		}
	}
	
	output.seekp(512 + 8 + 4);
	output.write(reinterpret_cast<char*>(&initrdcounter), sizeof(initrdcounter));
	
	output.seekp(20);
	short sum = initrdcounter + kernelcounter + counter + 1;
	output.write(reinterpret_cast<char*>(&sum), sizeof(sum));	// reserved sectors:
	// bootsector (1), stage 2 (counter), kernel (kernelcounter), initrd (initrdcounter)
	output.close();
	
	cout << "Bootfloppy created successfully." << endl;
	
	return 0;
}
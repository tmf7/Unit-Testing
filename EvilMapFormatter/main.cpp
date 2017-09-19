#include <fstream>
#include <vector>
#include <array>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Updates all master tileSet indexes based on the empty subframes from Tiled .tmx file
// EXAMPLE: tileset 1 has 24 fully used subframes
// tileset 2 has 60 used subframes, but Tiled says it has 64 subframes
// tileset 3 has 45 used subframes, but Tiles says it has 50 subframes
// THEREFORE: because each tileset is loaded IN-ORDER, just subtract the missing subframe amounts
// from each subsequent tileset value-range (ie: [0-23] leave alone, [24-60] leave alone, [61-106] subtract 4)
// (note the empty subframe indexes will never be used and never appear)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//*****************
// VerifyRead
// tests for unrecoverable read error or improperly formatted file
//*****************
bool VerifyRead(std::ifstream & read) {
	if (read.bad() || read.fail()) {
		read.clear();
		read.close();
		return false;
	}
	return true;
}

//*****************
// VerifyWrite
// tests for unrecoverable write error or improperly formatted file
//*****************
bool VerifyWrite(std::ofstream & write) {
	if (write.bad() || write.fail()) {
//		write.clear();
		write.close();
		return false;
	}
	return true;
}

//**************
// ReadUpdateRanges
// DEBUG (.rng file format):
// [minIndex0 maxIndex0 subtract0][minIndex1 maxIndex1 subtract1](repeat)\eof
//**************
bool ReadUpdateRanges(const char * rangesFilename, std::vector<std::array<int, 3>> & ranges) {
	std::ifstream	read(rangesFilename);
	// unable to find/open file
	if (!read.good()) 
		return false;

	while (!read.eof()) {
		read.ignore(1, '[');
		std::array<int, 3> range = { 0, 0, 0 };
		read >> range[0];		// min
		read >> range[1];		// max
		read >> range[2];		// how much to subtract from values in [min, max]
		read.ignore(1, ']');

		// let it crash
//		if (!VerifyRead(read))
//			return false;
		
		ranges.push_back(range);
	}
	read.close();
	return true;
}

//**************
// UpdateMapFile
// Updates the .tmx output file from Tiled map editor to .map format
// for consumption by the Engine-of-Evil
// DEBUG (.map file format **output**):
// master-tileSet-index, master-tileSet-index, ... master-tileSet-index\n
// # end of layer 1 comment\n
// master-tileSet-index, master-tileSet-index, ... master-tileSet-index\n
// # end of layer 2 comment\n
// (repeat, note that 0 as a master-tileSet-index indicates a placeholder, ie a tileMap index to skip for that layer)
// (and therefore ALL values should be reduced by 1 before setting an eTileImpl::type
// DEBUG: .map file format **input** is nearly identical to the **output** format
// except the input has a ',' before each '\n' (except the very last int for a layer)
//**************
bool UpdateMapFile(const char * mapFilename, const char * rangesFilename) {
	std::vector<std::array<int, 3>> ranges;
	if (!ReadUpdateRanges(rangesFilename, ranges))
		return false;

	std::ifstream	read(mapFilename);
	std::ofstream	write(std::string(mapFilename) + 'x');

	// unable to find/open file
	if (!read.good()) 
		return false;
	if (!write.good())
		return false;

	int layer = 1;
	while (!read.eof()) {
		int tileType = -2;
			
		// cap off the layer
		if (read.peek() == '#') {
			read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		} else {
			read >> tileType;

			if (!VerifyRead(read))
				return false;
		
			// [0] == min, [1] == max, [2] == amount to adjust
			for (auto range : ranges) {
				if (tileType >= range[0] && tileType <= range[1]) {
					tileType -= range[2];
					break;
				}
			}
			
			write << tileType;
			if (!VerifyWrite(write))
				return false;

			if (read.peek() == ',') {
				read.ignore(1, ',');
				if (read.peek() == '\n') {
					read.ignore(1, '\n');
					write << '\n';
				} else {
					write << (char)(',');		// DEBUG: non-cast ',' corrupts the tileType integer data
				}
			} else if (read.peek() == '\n') {
				read.ignore(1, '\n');
				write << "\n# end of layer " << layer << '\n';
				++layer;
			}

			if (!VerifyWrite(write))
				return false;
		}
	}
	read.close();
	return true;
}

//******************
// main
//*****************
int main(const char * argc) {
	if (!UpdateMapFile("MapsToFormat/EvilTown_u.map", "MapsToFormat/EvilTown.rng"))
		return 1;
	return 0;
}
#include <fstream>
#include <regex>

//*************************
// SpriteFrameFormatter
// Reads a Unity sprite/UI multi-type image.meta file
// that's been split into subframes
// and outputs a image subframe file consumable
// by the EngineOfEvil
// [output ".sub" file format] imageFilename\n
// numSubframes\n
// x y width height\t\t# 0\n	(clip-rectangle relative to top-left of image file; text beyond # will be ignored comment area in EngineOfEvil)
// x1 y1 width1 height1\t\t# 1\n
// (repeat)
//*************************
int main() {
	const auto noMatch = std::sregex_iterator();
	std::regex filenameRegex("[^\\/]+(?=\\..*$)", std::regex_constants::ECMAScript | std::regex_constants::icase);
	std::regex currentReadRegex("(rect:)", std::regex_constants::ECMAScript | std::regex_constants::icase);
	const char * toFormat = "SpriteFramesToFormat.txt";					// master file for all files to be formatted, in the same directory as executable
	const int MAX_STRING_LENGTH = 1024;
	char metaFilenameBuffer[MAX_STRING_LENGTH];
	char rectSearchBuffer[MAX_STRING_LENGTH];

	std::ifstream masterRead(toFormat, std::ios_base::in);
	while (!masterRead.eof()) {

		// get a file to read
		memset(metaFilenameBuffer, 0, sizeof(metaFilenameBuffer));
		masterRead.getline(metaFilenameBuffer, std::numeric_limits<std::streamsize>::max(), '\n');	// eg: "UnityMetas/sHero_Run_0.png.meta"
		std::ifstream currentRead(metaFilenameBuffer, std::ios_base::in);
		if (currentRead.bad() || currentRead.fail())						// unable to find meta file to read, skip it
			continue;

		// name to be added to top of output file contents (eg: sHero_Run_0.png)
		// regex for everything between the last '/' and last '.'
		std::string imageFilename(metaFilenameBuffer);
		auto firstMatch = std::sregex_iterator(imageFilename.begin(), imageFilename.end(), filenameRegex);
		if (firstMatch == noMatch)	// bad filename, skip it
			continue;

		imageFilename = firstMatch->str();

		// output file creation in graphics pre-existing directory within the executable directory
		std::string subframesFilename("EngineOutputs/");
		subframesFilename += imageFilename.substr(0, imageFilename.rfind('.'));
		subframesFilename += ".eimg";

		std::ofstream currentWrite(subframesFilename, std::ios_base::out);
		currentWrite << "Graphics/Textures/" << imageFilename.c_str() << '\n';
		currentWrite << "0 ";												// default texture type access to SDL_TEXTUREACCESS_STATIC
		auto subframeCountPosition = currentWrite.tellp();					// return here later to insert the subframesCount
		currentWrite << "\n\n";												// DEBUG: second newline will be overwritten by subframesCount

		int imageHeight = 0;
		int subframesCount = 0;

		currentRead >> imageHeight;											// Unity .meta file manually modified to have imageHeight as first entry (because the .meta doesn't contain the true image height)

		while (!currentRead.eof()) {

			// search for an instance of "rect:"
			while(!currentRead.eof()) {
				currentRead.getline(rectSearchBuffer, std::numeric_limits<std::streamsize>::max(), '\n');
				std::string rectSearch(rectSearchBuffer);
				auto firstMatch = std::sregex_iterator(rectSearch.begin(), rectSearch.end(), currentReadRegex);
				if (firstMatch != noMatch)
					break;
			}

			// quit if either NO "rect:" exists in the file, or all "rect:"s have been found
			if (currentRead.eof())
				break;

			// skip to end of "x:" after "rect:"
			currentRead.ignore(std::numeric_limits<std::streamsize>::max(), ':');
			currentRead.ignore(std::numeric_limits<std::streamsize>::max(), ':');

			int frameX = 0;
			int frameY = 0;
			int frameWidth = 0;
			int frameHeight = 0;
			currentRead >> frameX;
			currentRead.ignore(std::numeric_limits<std::streamsize>::max(), ':');
			currentRead >> frameY;
			currentRead.ignore(std::numeric_limits<std::streamsize>::max(), ':');
			currentRead >> frameWidth;
			currentRead.ignore(std::numeric_limits<std::streamsize>::max(), ':');
			currentRead >> frameHeight;

			frameY = imageHeight - (frameHeight + frameY);			// convert bottom-left Unity rect origins to top-left Engine-of-Evil rect origins

			// output
			currentWrite << frameX << ' ' << frameY << ' ' << frameWidth << ' ' << frameHeight << "\t\t# " << subframesCount << '\n';
			++subframesCount;
		}
		currentWrite.seekp(subframeCountPosition);
		currentWrite << subframesCount << '\n';
		currentWrite.seekp(-1, std::ios::end);
		currentWrite << " eof";
		currentWrite.close();
		currentRead.close();
	}
	masterRead.close();
	return 0;
}
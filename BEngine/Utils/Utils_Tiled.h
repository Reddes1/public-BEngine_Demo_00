//*********************************************************************************\\
// 
// Support functionality for loading maps from Tiled program.
//
//*********************************************************************************\\

#pragma once

class Utils_Tiled
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Bits on the far end of the 32-bit global tile ID are used for tile flags
	static const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
	static const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
	static const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;
	static const unsigned ROTATED_HEXAGONAL_120_FLAG = 0x10000000;

	//////////////////
	/// Operations ///
	//////////////////
	
	//Removes the flags from an ID and returms them
	static unsigned RemoveFlags(unsigned id)
	{
		//Clears known flags from the ID
		id &= ~(FLIPPED_HORIZONTALLY_FLAG |
			FLIPPED_VERTICALLY_FLAG |
			FLIPPED_DIAGONALLY_FLAG |
			ROTATED_HEXAGONAL_120_FLAG);

		return id;
	}

	//Checks for horizontal flip flag and returns result
	static bool IsFlippedHorizontally(unsigned id)
	{
		return (id & FLIPPED_HORIZONTALLY_FLAG);
	}
	//Checks for vertical flip flag and returns result
	static bool IsFlippedVertically(unsigned id)
	{
		return (id & FLIPPED_VERTICALLY_FLAG);
	}
	//Checks for diagonal flip flag and returns result
	static bool IsFlippedDiagonally(unsigned id)
	{
		return (id & FLIPPED_DIAGONALLY_FLAG);
	}
	//Checks for hex120 rotation flag and returns result
	static bool IsRotatedHex120(unsigned id)
	{
		return (id & ROTATED_HEXAGONAL_120_FLAG);
	}
};
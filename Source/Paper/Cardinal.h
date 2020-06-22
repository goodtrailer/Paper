// Copyright (c) 2019�2020 Alden Wu

#pragma once
#include "PaperEnums.h"
#include "Cardinal.generated.h"

/**
 * Cardinal of booleans.
 *
 * Simple wrapper for TArray that holds booleans for each cardinal direction: Up, Right, Down, Left.
 * Meant for use with EDirection, which is used to represent the cardinal directions, but can just
 * as easily be used with uint8 without need for static_cast.
 */
USTRUCT(BlueprintType)
struct FCardinal
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<bool> Directions;

	/**
	* Individual constructor. Sets Directions with specified values for directions.
	*
	* @param	Up		Boolean value for Up (index 0) in Directions.
	* @param	Right	Boolean value for Right (index 1) in Directions.
	* @param	Down	Boolean value for Down (index 2) in Directions.
	* @param	Left	Boolean value for Left (index 3) in Directions.
	*/
	FCardinal(const bool Up, const bool Right, const bool Down, const bool Left)
	{
		Directions.Add(Up);
		Directions.Add(Right);
		Directions.Add(Down);
		Directions.Add(Left);
	}

	/**
	* Array constructor. Sets Directions with an array for specified values for directions.
	*
	* @param	Array		C-style array of booleans to set Directions to.
	*/
	FCardinal(const bool Array[4])
	{
		Directions = TArray<bool>(Array, 4);
	}

	/**
	* Copy constructor.
	*
	* @param	Source	The FCardinal to copy from.
	*/
	FCardinal(const FCardinal& Source)
	{
		Directions = TArray<bool>(Source.Directions);
	}

	/**
	 * Empty constructor. Fills Directions with false;
	 */
	FCardinal()
	{
		Directions.Init(false, 4);
	}

	/**
	 * Array bracket operator. Returns boolean of given Direction.
	 *
	 * @param	Direction	Direction to get the boolean value of.
	 */
	bool& operator[](const EDirection Direction)
	{
		return Directions[static_cast<uint8>(Direction)];
	}

	/**
	 * Array bracket operator (const version). Returns boolean of given Direction.
	 *
	 * @param	Direction	Direction to get the boolean value of.
	 */
	const bool& operator[](const EDirection Direction) const
	{
		return Directions[static_cast<uint8>(Direction)];
	}

	/**
	 * Array bracket operator. Returns boolean of Direction given as a uint8.
	 *
	 * @param	Direction	Direction as a uint8 to get the boolean value of.
	 */
	bool& operator[](const uint8 Direction)
	{
		return Directions[Direction];
	}

	/**
	 * Array bracket operator (const version). Returns boolean of Direction given as a uint8.
	 *
	 * @param	Direction	Direction as a uint8 to get the boolean value of.
	 */
	const bool& operator[](const uint8 Direction) const
	{
		return Directions[Direction];
	}

	/**
	 * Equivalency operator. Returns true if Directions are equivalent.
	 *
	 *@param	Other		The other FCardinal to be compared to.
	 */
	bool operator==(const FCardinal& Other) const
	{
		return Directions == Other.Directions;
	}
};

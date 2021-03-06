// Copyright (c) 2019–2020 Alden Wu


#include "TruncatedPrism.h"
#include "GenericPlatform/GenericPlatformMath.h"

UTruncatedPrism::UTruncatedPrism()
{
	Diameter = 75.f;
	Thickness = 9.2f;
	StartingFraction = 1.f;
	CurrentFraction = -1.f;
	SideCount = 8;
	bIncludeBottomFace = false;
}

void UTruncatedPrism::BeginPlay()
{
	Super::BeginPlay();
	Truncate(StartingFraction);
}

void UTruncatedPrism::Truncate(float Fraction)
{
	if (Fraction == CurrentFraction || Fraction < 0)
		return;

	if (Fraction == 0 || SideCount < 3)
	{
		ClearCustomMeshTriangles();
		return;
	}


	const float SectionAngle = 2 * PI / SideCount;			// A section is a triangle, and n-gons have n sections
	const float DiagonalRadius = Diameter / (2 * FPlatformMath::Cos(SectionAngle / 2));
	
	TArray<FVector> Vertices;
	
	if (bTruncateZ)
	{
		const float MaxZ = FPlatformMath::Min(Thickness * Fraction, Thickness);		// the max allowed z value for a point, for truncation.
		
		// Calculate the regular n-gon using diagonals, truncating at Z
		for (int i = 0; i < (SideCount + 1) / 2; i++)
			Vertices.Emplace(
				DiagonalRadius * FPlatformMath::Cos(SectionAngle * (i + 0.5f)),
				DiagonalRadius * FPlatformMath::Sin(SectionAngle * (i + 0.5f)),
				MaxZ
			);

		// Calculate the other half of the n-gon by mirroring the X, using MaxZ
		for (int i = Vertices.Num() - 1 - SideCount % 2; i > -1; i--)
			Vertices.Emplace(Vertices[i].X, -Vertices[i].Y, MaxZ);
	}
	else
	{
		bool Truncated = false;
		const float MinX = Diameter / 2 - Fraction * Diameter;	// the min allowed x value for a point, for truncation. (points are calculated positive-to-negative, which is why we use min and not max)
		for (int i = 0; i < (SideCount + 1) / 2; i++)
		{
			const float ProposedX = DiagonalRadius * FPlatformMath::Cos(SectionAngle * (i + 0.5f));
			// if this proposed point does not exceed the min, then don't truncate it or apply any sort of calculations
			if (MinX <= ProposedX)
				Vertices.Emplace(
					ProposedX,
					DiagonalRadius * FPlatformMath::Sin(SectionAngle * (i + 0.5f)),
					Thickness
				);
			// if this proposed point's x exceeds the min, then truncate the Y value so that it matches the min
			else
			{
				Vertices.Emplace(
					MinX,
					Vertices.Last().Y - (MinX - Vertices.Last().X) / (ProposedX - Vertices.Last().X) * (Vertices.Last().Y - DiagonalRadius * FPlatformMath::Sin(SectionAngle * (i + 0.5f))),
					Thickness
				);
				Truncated = true;
				break;
			}
		}

		// Calculate the other half of the n-gon by mirroring the X
		uint8 skip = 1;
		if (SideCount % 2 && !Truncated)
			skip = 2;
		for (int i = Vertices.Num() - skip; i > -1; i--)
			Vertices.Emplace(Vertices[i].X, -Vertices[i].Y, Thickness);
	}

	


	TArray<FCustomMeshTriangle> Triangles;
	// tri-counts for an n-gon:
	// top face: n - 2
	// ring faces: n * 2
	Triangles.Reserve(Vertices.Num() * 3 - 2);		// add top and side faces


	// Triangulate the top face using the fan-method, with the 0th vertex as the anchor
	for (int i = 1; i < Vertices.Num() - 1; i++)
	{
		// FCustomMeshTriangle doesn't have a constructor that takes in 3 vectors (epic games ??)	
		FCustomMeshTriangle TempTriangle;
		TempTriangle.Vertex0 = Vertices[0];
		TempTriangle.Vertex1 = Vertices[i + 1];
		TempTriangle.Vertex2 = Vertices[i];
		Triangles.Emplace(MoveTemp(TempTriangle));
	}

	if (bIncludeBottomFace)
	{
		for (int i = 1; i < Vertices.Num() - 1; i++)
		{
			FCustomMeshTriangle TempTriangle;
			TempTriangle.Vertex0 = FVector(Vertices[0].X, Vertices[0].Y, 0.f);
			TempTriangle.Vertex1 = FVector(Vertices[i].X, Vertices[i].Y, 0.f);
			TempTriangle.Vertex2 = FVector(Vertices[i + 1].X, Vertices[i + 1].Y, 0.f);
			Triangles.Emplace(MoveTemp(TempTriangle));
		}
	}

	// Calculate the ring of the prism
	// Note: this is a for-loop unwrapped as a while-loop, so that i is still accessible after the loop finishes
	{
		int i = 0;
		while (i < Vertices.Num() - 1)
		{
			//	imagine a square:
			//	V[i]-----V[i + 1]
			//	| \       |
			//	|    \    |
			//	|       \ |
			//	B0-------B1

			FCustomMeshTriangle Triangle1, Triangle2;
			Triangle1.Vertex0 = Vertices[i];												// V[i]
			Triangle1.Vertex1 = Vertices[i + 1];											// V[i + 1]
			Triangle1.Vertex2 = FVector(Vertices[i + 1].X, Vertices[i + 1].Y, 0.f);			// B1

			Triangle2.Vertex0 = Vertices[i];												// V[i]
			Triangle2.Vertex1 = FVector(Vertices[i + 1].X, Vertices[i + 1].Y, 0.f);			// B1
			Triangle2.Vertex2 = FVector(Vertices[i].X, Vertices[i].Y, 0.f);					// B0
			
			Triangles.Emplace(MoveTemp(Triangle1));
			Triangles.Emplace(MoveTemp(Triangle2));
			i++;
		}
		FCustomMeshTriangle Triangle1, Triangle2;
		Triangle1.Vertex0 = Vertices[i];
		Triangle1.Vertex1 = Vertices[0];
		Triangle1.Vertex2 = FVector(Vertices[0].X, Vertices[0].Y, 0.f);
		Triangle2.Vertex0 = Vertices[i];
		Triangle2.Vertex1 = FVector(Vertices[0].X, Vertices[0].Y, 0.f);
		Triangle2.Vertex2 = FVector(Vertices[i].X, Vertices[i].Y, 0.f);
		Triangles.Emplace(MoveTemp(Triangle1));
		Triangles.Emplace(MoveTemp(Triangle2));
	}

	SetCustomMeshTriangles(Triangles);
	CurrentFraction = Fraction;
}

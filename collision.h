//collision.h: contains collision code for use with SFML
//
//yes, this is a header file that includes implementation of code. sue me
//
#ifndef COLLISION_H
#define COLLISION_H

#include <SFML/ConvexShape.hpp>
#include <SFML/CircleShape.hpp>
#include <SFML/Vector2.hpp>
#include <cmath> //this was supposed to be easy

#define PI 3.14159265 //I was a fool


static class Collision {

	public:
	struct colData{
		bool didHit;
		sf::Vector2f POI;
	}
	
	//could be extended to support collisions between cShapes & cShapes
	//or of course circles and circles, but that's kinda trivial
	colData isShapeCollidingWithCircle(sf::ConvexShape, sf::CircleShape);
}


colData Collision::isCollidingWithCircle(sf::ConvexShape shape, sf::CircleShape circle){
	//determine if the given circle is colliding with the given convex shape
	sf::Vector2f cPos = circle.getPosition();
	sf::Vector2f sPos = shape.getPosition();
	
	//delta vector: from shape to circle
	sf::Vector2f delta = cPos-sPos;

	//angle of vector from shape origin to circle origin
	float phi = atan2(delta.y, delta.x);
	phi += 2.0f * PI;	//just trust me on this one

	int N = shape.getPointCount();
	sf::Vector2f thisPoint;
	sf::Vector2f prevPoint;
	sf::Vector2f pointDelta;
	float thisPhi;
	float prevPhi;
	bool foundFacing = false;
	//currently rotating colliders is not supported, so please don't
	//for each point in our shape:
	for (int i=0; i<N; i++){
		thisPoint = shape.getPoint(i);
		pointDelta = thisPoint-sPos;
		thisPhi = atan2(pointDelta.y, pointDelta.x);
		
		if (i!=0){
			//is the circle between this point and the previous point?
			thisPhi += 2.0f * PI;
			prevPi += 2.0f * PI;
			//Q: do I need to determine if this is the closest face? (RE: I don't think so)
			if (thisPhi > prevPhi) //CCW points
				foundFacing = (thisPhi > phi && phi > prevPhi);
			else //clockwise points
				foundFacing = (prevPhi > phi && phi > thisPhi);
			if (foundFacing)
				break;
		}
		
		prevPoint = thisPoint;
		prevPhi = thisPhi;
	}
	
	//prepare to return collision data	
	colData data;
	if (foundFacing){
		//the path to our circle lies between thisPoint and prevPoint.
		//find POI of: face btwn two points, and path from shape to circle.
		//formulas below are derived from basic algebra. it's y=mx+b up in here :fire:
		sf::Vector2f POI;
		float slopeFace = (thisPoint.y-prevPoint.y)/(thisPoint.x/thisPoint.x);
		float slopePath = (sPos.y-cPos.y)/(sPos.x-cPos.x);
		float bFace = thisPoint.y - slopeFace * thisPoint.x;
		float bPath = sPos.y - slopePath * sPos.x;
		POI.x = (bFace-bPath)/(slopePath-slopeFace); //that's fucking adorable
		POI.y = slopeFace * POI.x + bFace;

		//the moment of truth: is POI within the circle?
		float dist = hypot(POI.x-cPos.x, POI.y-cPos.y)); //hypot is c++11
		colData.didHit = (circle.getRadius() >= dist); //TODO: circle and shape are overlapping a tiny bit
		colData.POI = POI;
	}
	else colData.didHit = false;
	return colData;
}

#endif
Incomplete, but functional rigid body physics engine developed by me in early 2016. Some of the architectural decisions were influenced Erin Catto's Box2D.

* Uses the GJK distance algorithm to handle collisions between arbitrary convex polygons.
* Angular momentums are accounted for during collision response.
* Restitution;
* Static and dynamic friction;
* Pretty fast, would be even faster after implementing space partitioning!
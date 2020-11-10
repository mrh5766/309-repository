There are two significant errors I'm not sure how to fix in my code. 

1. Octree levels 3+ give false negative collisions. My guess would be that entities aren't checked against all entities in children of the octree, but I'm not sure why.
I've looked over my CheckChildCollisions method many times, but I'm not seeing the error.

2. Octree octants don't divide properly based on the ideal number of entities in them - this is evident at levels 3+. Octants don't divide sometimes even when I set
idealEntityCount to 1 and they clearly have more than 1 entity in them, and again, I'm not sure why.

The controls are page up and page down to hide or display octree octants except the outermost one, and + and - to increase or decrease octree levels.
Level 1 is just a list - I get about 10 fps on it, and 30 on octree level 2.
Initial code source - https://github.com/jyanar/Boids

Added 2 engines
1. basic - O(n^2) --> supports 700-800 boids at 30fps
2. quadtree - O(nlogn) --> supports 5000-6000 boids at 30fps
... further plans to add gpu optimization ...
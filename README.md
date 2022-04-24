ECE 568 Final Project
===

Docker Configurations: 
---
### 1. UPS and World hostname
To change the hostname of UPS and world, modify the docker-compose.yml file, change the HOST_UPS under server -> build -> args. Docker should pass this argument into the server container, the Dockerfile under ./server/ will make this argument an environment variable, visible from inside the run.sh.

	![Untitled](https://user-images.githubusercontent.com/49318361/164995380-23c76d80-203d-4853-a41f-410160bc4958.png)

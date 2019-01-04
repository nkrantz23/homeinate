deploy:
	netlify deploy --dir=web --functions=lambda/build

promote:
	netlify deploy --prod --dir=web --functions=lambda/build

build-functions:
	rm -rf lambda/build/*
	cd lambda && npm run build
	
build: build-functions
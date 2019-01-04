deploy:
	netlify deploy --dir=web --functions=lambda/build

promote:
	netlify deploy --prod --dir=web --functions=lambda/build
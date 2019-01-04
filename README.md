homeinate
------

dominate your domicile with hominate

## deploying

1. Setup netlify-cli and auth to your Netlify account: https://www.netlify.com/docs/cli/
2. `make deploy` preview your deploy.
3. `make promote` deploy to production.

## notes

1. Your lambda's must be at the top-level in `lambda` (you can use nested directories for includes).
2. Your lambda's will be at `{your-netlify-url}/.netlify/functions/{file_name}`
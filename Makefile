start:
	npm start

# disable implicit cleanup after each test for react-testing-library with RTL_SKIP_AUTO_CLEANUP=true

test:
	npm run jest

test-watch:
	npm run jest -- --watchAll

test-coverage:
	npm run jest -- --coverage

webpack:
	NODE_ENV=production npm run webpack

update-babel:
	npm uninstal @babel/core && \
	npm uninstall @babel/preset-env && \
	npm i -D @babel/core && \
	npm i -D @babel/preset-env

update-webpack:
	npm uninstall webpack && \
	npm uninstall webpack-cli && \
	npm uninstall webpack-dev-server && \
	npm i -D webpack && \
	npm i -D webpack-cli && \
	npm i -D webpack-dev-server

update-webpack-plugins:
	npm uninstall clean-webpack-plugin && \
	npm uninstall html-webpack-plugin && \
	npm uninstall css-loader && \
	npm uninstall style-loader && \
	npm i -D clean-webpack-plugin && \
	npm i -D html-webpack-plugin && \
	npm i -D css-loader && \
	npm i -D style-loader

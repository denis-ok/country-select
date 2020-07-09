const HtmlWebpackPlugin = require("html-webpack-plugin");
const { CleanWebpackPlugin } = require("clean-webpack-plugin");

const path = require("path");

const outputDir = path.join(__dirname, "demo", "build");
const publicDir = path.join(__dirname, "demo", "public");

const isProd = process.env.NODE_ENV === "production";

module.exports = {
  entry: "./demo/Index.bs.js",
  mode: isProd ? "production" : "development",
  output: {
    path: outputDir,
    filename: "bundle.js",
  },
  module: {
    rules: [
      {
        test: /\.css$/,
        use: [
          {
            loader: "style-loader",
          },
          {
            loader: "css-loader",
            options: {
              modules: {
                localIdentName: "[local]-[hash:base64]",
              },
            },
          },
        ],
      },
    ],
  },
  plugins: [
    new CleanWebpackPlugin(),
    new HtmlWebpackPlugin({
      template: path.join(__dirname, "demo/index.html"),
    }),
  ],

  devServer: {
    contentBase: publicDir,
    historyApiFallback: true,
  },
};

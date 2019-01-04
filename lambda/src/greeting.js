const { Slack } = require('./lib/slack');
const { Response } = require('./lib/http');

exports.handler = async (event, context, callback) => {
  return new Promise((resolve, reject) => {
    resolve(
      Response.success(
        Slack.inChannel(`Hello. I am ready to dominate your domicile.`)
      )
    );
  });
};

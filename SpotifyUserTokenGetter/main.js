const creds = require("./creds");
const express = require("express");
const querystring = require("querystring");
const axios = require("axios");
const crypto = require("crypto");

const app = express();
const redirect_uri = "http://localhost:8888/callback";

app.get("/login", function (req, res) {
  const state = crypto.randomBytes(16).toString("hex"); // Securely generate a random state
  const scope = "user-read-private user-read-email user-read-currently-playing";

  res.redirect(
    "https://accounts.spotify.com/authorize?" +
      querystring.stringify({
        response_type: "code",
        client_id: creds.CLIENT_ID,
        scope: scope,
        redirect_uri: redirect_uri,
        state: state,
      })
  );
});

app.get("/callback", async function (req, res) {
  const code = req.query.code || null;
  const state = req.query.state || null;

  if (state === null) {
    res.redirect(
      "/#" +
        querystring.stringify({
          error: "state_mismatch",
        })
    );
  } else {
    const authOptions = {
      url: "https://accounts.spotify.com/api/token",
      data: querystring.stringify({
        code: code,
        redirect_uri: redirect_uri,
        grant_type: "authorization_code",
      }),
      headers: {
        "Content-Type": "application/x-www-form-urlencoded",
        Authorization:
          "Basic " +
          Buffer.from(creds.CLIENT_ID + ":" + creds.CLIENT_SECRET).toString(
            "base64"
          ),
      },
    };

    const response = await axios.post(authOptions.url, authOptions.data, {
      headers: authOptions.headers,
    });
    console.log(response.data);
  }
});

app.listen(8888, () => {
  console.log("Server is running on http://localhost:8888/login");
});

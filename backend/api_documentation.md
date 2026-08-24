# Backend API Documentation

API specification for the backend service. All request payloads and non-empty responses use `application/json`.

---

## Table of Contents

* [Health Check](#get-apihealth_check)
* [Start Game](#post-apistart_game)
* [Get Current Round](#post-apiget_round)
* [Submit Guess](#post-apisubmit_guess)
* [Skip Round](#post-apiskip_round)
* [Create Challenge](#post-apichallenges)
* [Get Challenge](#get-apichallengesuuid)
* [Submit Challenge Attempt](#post-apichallengesuuidattempts)

---

### `GET /api/health_check`

Checks the operational status of the API server.

#### Request
* **Headers:** `Content-Type: application/json`

#### Response

* **`200 OK`**
  ```json
  {
    "status": "ok"
  }
  ```

---

### `POST /api/start_game`

Initializes a new game session, selects random target coordinates/images, and returns the first round details.

#### Request Body
| Field | Type | Required | Description |
| :--- | :--- | :--- | :--- |
| `totalRounds` | `integer` | Yes | The requested number of rounds for the game session. Clamped automatically to valid bounds `[1, MAX_ROUNDS]`. |

```json
{
  "totalRounds": 5
}
```

#### Response

* **`200 OK`** (Game Created)
  ```json
  {
    "sessionId": "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
    "totalRounds": 5,
    "round": 1,
    "imageUrl": "https://storage.googleapis.com/your-bucket/image.jpg"
  }
  ```

* **`400 Bad Request`** (Invalid or missing parameters)
  ```json
  {
    "error": "Malformed JSON payload"
  }
  ```
  *or*
  ```json
  {
    "error": "Invalid totalRounds value"
  }
  ```

---

### `POST /api/get_round`

Retrieves details and image for the current active round of an existing session.

#### Request Body
| Field | Type | Required | Description |
| :--- | :--- | :--- | :--- |
| `sessionId` | `string` | Yes | The active session ID. |

```json
{
  "sessionId": "a1b2c3d4-e5f6-7890-abcd-ef1234567890"
}
```

#### Response

* **`200 OK`** (Active Round)
  ```json
  {
    "round": 2,
    "totalRounds": 5,
    "imageUrl": "https://storage.googleapis.com/your-bucket/image.jpg"
  }
  ```

* **`200 OK`** (Game Already Over)
  ```json
  {
    "error": "Game is already over",
    "gameOver": true,
    "totalScore": 14250
  }
  ```

* **`400 Bad Request`**
  ```json
  {
    "error": "Missing sessionId"
  }
  ```

* **`404 Not Found`**
  ```json
  {
    "error": "Session not found or expired"
  }
  ```

---

### `POST /api/submit_guess`

Submits coordinate guesses for the current round, calculates distance-based score, and reveals the actual location coordinates.

#### Request Body
| Field | Type | Required | Description |
| :--- | :--- | :--- | :--- |
| `sessionId` | `string` | Yes | The active session ID. |
| `lat` | `float` | Yes | Player's guessed latitude coordinate. |
| `lng` | `float` | Yes | Player's guessed longitude coordinate. |

```json
{
  "sessionId": "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
  "lat": 33.6405,
  "lng": -117.8443
}
```

#### Response

* **`200 OK`** (Guess Submitted)
  ```json
  {
    "score": 4950,
    "answerLat": 33.6410,
    "answerLng": -117.8450,
    "totalScore": 9800,
    "round": 1,
    "gameOver": false
  }
  ```

* **`400 Bad Request`**
  ```json
  {
    "error": "Missing sessionId, lat, or lng"
  }
  ```
  *or*
  ```json
  {
    "error": "This round was already submitted"
  }
  ```

* **`404 Not Found`**
  ```json
  {
    "error": "Session not found or expired"
  }
  ```

---

### `POST /api/skip_round`

Skips the current round without submitting coordinate guesses. Awards `0` points for the round and reveals the actual location.

#### Request Body
| Field | Type | Required | Description |
| :--- | :--- | :--- | :--- |
| `sessionId` | `string` | Yes | The active session ID. |

```json
{
  "sessionId": "a1b2c3d4-e5f6-7890-abcd-ef1234567890"
}
```

#### Response

* **`404 Not Found`**
  ```json
  {
    "error": "Session not found or expired"
  }
  ```

---

### `POST /api/challenges`

Creates an async 1v1 challenge with a shared, ordered set of images. The creator should play as `role: "creator"`; the shared link is for `invitee`.

#### Request Body
| Field | Type | Required | Description |
| :--- | :--- | :--- | :--- |
| `totalRounds` | `integer` | No | Defaults to `5`. Clamped to `[1, MAX_ROUNDS]`. |

```json
{
  "totalRounds": 8
}
```

#### Response

* **`201 Created`**
  ```json
  {
    "challengeId": "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
    "totalRounds": 8
  }
  ```

---

### `GET /api/challenges/<uuid>`

Returns challenge images (without answer coordinates) and both players' attempt status.

#### Response

* **`200 OK`**
  ```json
  {
    "challengeId": "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
    "totalRounds": 8,
    "images": [
      { "displayOrder": 1, "imageUrl": "https://storage.googleapis.com/..." }
    ],
    "attempts": {
      "creator": null,
      "invitee": { "completed": true, "totalScore": 18200 }
    }
  }
  ```

* **`404 Not Found`**
  ```json
  {
    "error": "Challenge not found"
  }
  ```

---

### `POST /api/challenges/<uuid>/attempts`

Submits one player's full set of guesses. Exactly one `creator` and one `invitee` attempt are allowed. Omitted rounds score `0`. Answers are returned only in this response.

#### Request Body
```json
{
  "role": "invitee",
  "guesses": [
    { "displayOrder": 1, "lat": 33.6405, "lng": -117.8443 }
  ]
}
```

#### Response

* **`200 OK`**
  ```json
  {
    "success": true,
    "totalScore": 18200,
    "roundBreakdowns": [
      {
        "displayOrder": 1,
        "score": 4950,
        "answerLat": 33.6410,
        "answerLng": -117.8450
      }
    ]
  }
  ```

* **`409 Conflict`** — that role already submitted.
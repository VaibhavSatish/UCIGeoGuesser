// components/GameApp.tsx
"use client";

import React, { useState, useEffect, useRef } from "react";
// Removed: idb import (no more client-side caching of answer data)
// Removed: calculateScore import (scoring now happens server-side)

import Results from "../results";
import Guess from "../guess";
import GameTimer from "../GameTimer";
import GameOver from "../GameOver";

export default function GameApp() {
  /* Backend URL */
  const backendUrl = "http://0.0.0.0:18080";

  /*Use States*/
  const [loading, setLoading] = useState<boolean>(true);
  const [imageSrc, setImageSrc] = useState<string>("");
  // Removed: locationData — answer coords no longer stored client-side
  const [guessCoords, setGuessCoords] = useState<[number, number] | null>(null);
  const [isHovering, setIsHovering] = useState<boolean>(false);
  const [hasGuessed, setHasGuessed] = useState<boolean>(false);

  /* Session state (new) */
  const [sessionId, setSessionId] = useState<string | null>(null);
  const [roundScore, setRoundScore] = useState<number | null>(null);
  const [answerCoords, setAnswerCoords] = useState<{ lat: number; lng: number } | null>(null);

  /*Map iframe ref*/
  const iframeRef = useRef<HTMLIFrameElement | null>(null);

  /*Round / timer*/
  const mapZoom = 14.5;
  const timeLimit = 60; //seconds
  const maxRounds = 8;
  const [currRound, setCurrRound] = useState<number>(0);
  const [finalScore, setFinalScore] = useState<number>(0);
  const [gameOver, setGameOver] = useState<boolean>(false);

  /* helper to send message to iframe safely */
  const sendToMap = (msg: any) => {
    const win = iframeRef.current?.contentWindow;
    if (win) {
      win.postMessage(msg, "*"); // '*' is ok for dev; restrict origin in prod
    }
  };

  /* Start a new game by calling the backend */
  const startGame = async () => {
    setLoading(true);
    setHasGuessed(false);
    setGuessCoords(null);
    setRoundScore(null);
    setAnswerCoords(null);

    try {
      const res = await fetch(`${backendUrl}/api/start_game`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ totalRounds: maxRounds }),
      });

      if (!res.ok) {
        const errData = await res.json().catch(() => null);
        throw new Error(errData?.error || `Server error: ${res.status}`);
      }

      const data = await res.json();
      console.log("Game started:", data);

      setSessionId(data.sessionId);
      setCurrRound(data.round);
      setImageSrc(data.imageUrl);
      setLoading(false);

      // Clear iframe map markers for new game
      sendToMap({
        type: "clear",
        center: [33.645934402549955, -117.84272074704859],
        zoom: mapZoom,
      });
    } catch (err) {
      console.error("Failed to start game:", err);
      setImageSrc("");
      setLoading(true);
    }
  };

  /* Load the next round from the backend */
  const loadNextRound = async () => {
    if (!sessionId) return;

    setLoading(true);
    setHasGuessed(false);
    setGuessCoords(null);
    setRoundScore(null);
    setAnswerCoords(null);

    try {
      const res = await fetch(`${backendUrl}/api/get_round`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ sessionId }),
      });

      if (!res.ok) {
        const errData = await res.json().catch(() => null);
        throw new Error(errData?.error || `Server error: ${res.status}`);
      }

      const data = await res.json();

      if (data.gameOver) {
        setGameOver(true);
        setFinalScore(data.totalScore);
        setLoading(false);
        return;
      }

      setCurrRound(data.round);
      setImageSrc(data.imageUrl);
      setLoading(false);

      // Clear map for new round
      sendToMap({
        type: "clear",
        center: [33.645934402549955, -117.84272074704859],
        zoom: mapZoom,
      });
    } catch (err) {
      console.error("Failed to load round:", err);
    }
  };

  /* Submit a guess to the backend */
  const submitGuess = async (lat: number, lng: number) => {
    if (!sessionId) return;

    try {
      const res = await fetch(`${backendUrl}/api/submit_guess`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ sessionId, lat, lng }),
      });

      if (!res.ok) {
        const errData = await res.json().catch(() => null);
        throw new Error(errData?.error || `Server error: ${res.status}`);
      }

      const data = await res.json();
      console.log("Guess result:", data);

      // Now we receive the answer coordinates from the server
      setRoundScore(data.score);
      setFinalScore(data.totalScore);
      setAnswerCoords({ lat: data.answerLat, lng: data.answerLng });
      setHasGuessed(true);

      if (data.gameOver) {
        setGameOver(true);
      }

      // Show answer marker on map (answer coords are now safe to use — round is over)
      sendToMap({
        type: "lockAndShowAnswer",
        lat: data.answerLat,
        lng: data.answerLng,
      });
    } catch (err) {
      console.error("Failed to submit guess:", err);
    }
  };

  /* Skip a round (timer expired without a guess) */
  const skipRound = async () => {
    if (!sessionId) return;

    try {
      const res = await fetch(`${backendUrl}/api/skip_round`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ sessionId }),
      });

      if (!res.ok) {
        const errData = await res.json().catch(() => null);
        throw new Error(errData?.error || `Server error: ${res.status}`);
      }

      const data = await res.json();
      console.log("Round skipped:", data);

      setRoundScore(0);
      setFinalScore(data.totalScore);
      setAnswerCoords({ lat: data.answerLat, lng: data.answerLng });
      setHasGuessed(true);

      if (data.gameOver) {
        setGameOver(true);
      }

      // Show answer on map
      sendToMap({
        type: "lockAndShowAnswer",
        lat: data.answerLat,
        lng: data.answerLng,
      });
    } catch (err) {
      console.error("Failed to skip round:", err);
    }
  };

  /* Initial load */
  useEffect(() => {
    startGame();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  /* Keyboard handlers (Space to lock guess, Enter to go to next round) */
  useEffect(() => {
    const KeyPressHandler = (event: KeyboardEvent) => {
      if (event.code === "Space" && guessCoords && !hasGuessed) {
        event.preventDefault();
        submitGuess(guessCoords[0], guessCoords[1]);
      } else if (event.code === "Enter" && hasGuessed && !gameOver) {
        event.preventDefault();
        loadNextRound();
      }
    };

    window.addEventListener("keydown", KeyPressHandler);
    return () => window.removeEventListener("keydown", KeyPressHandler);
  }, [guessCoords, hasGuessed, sessionId, gameOver]);

  /* Message from iframe (map) -> parent */
  useEffect(() => {
    function onMessage(ev: MessageEvent) {
      const msg = ev.data || {};
      if (!msg || !msg.type) return;

      switch (msg.type) {
        case "mapReady":
          break;

        case "guess":
          if (typeof msg.lat === "number" && typeof msg.lng === "number") {
            setGuessCoords([msg.lat, msg.lng]);
          }
          break;

        default:
          break;
      }
    }
    window.addEventListener("message", onMessage);
    return () => window.removeEventListener("message", onMessage);
  }, []);

  /* Handle "Next Image" from Results component */
  const handleNextImageFromResults = () => {
    loadNextRound();
  };

  return (
    <div
      className="min-h-screen w-full relative transition-opacity duration-500"
      style={{
        backgroundImage: `url(${imageSrc})`,
        backgroundSize: "cover",
        backgroundPosition: "center",
        backgroundColor: "#0f172a",
      }}
    >
      {loading && (
        <div className="absolute inset-0 bg-black bg-opacity-70 flex items-center justify-center z-10">
          <div className="w-16 h-16 border-4 border-white border-t-transparent rounded-full animate-spin" />
        </div>
      )}

      {!loading && (
        <div className="min-h-screen flex flex-col items-center justify-center">
          {/* Top HUD */}
          <div className="absolute top-2 left-2 bg-gray-500/30 bg-opacity-90 px-2 py-1 rounded-2xl shadow-xl text-center w-full max-w-xs">
            <div className="flex justify-between items-center">
              <h1 className="text-white font-extrabold text-4xl drop-shadow-[px_1px_0px_black]">
                UCI GeoGuesser
              </h1>
              <div className="text-white text-lg">
                <div className="text-white">
                  <span className="font-bold">Round: </span>
                  <span>
                    {" "}
                    {currRound}/{maxRounds}{" "}
                  </span>
                </div>
              </div>
            </div>

            <div className="mt-2 text-white">
              {!gameOver && !hasGuessed ? (
                <GameTimer
                  timeLimitInSeconds={timeLimit}
                  onEnd={() => {
                    if (!guessCoords) {
                      // Timer expired with no guess — skip the round
                      skipRound();
                    } else {
                      // Timer expired but user had placed a pin — submit their guess
                      submitGuess(guessCoords[0], guessCoords[1]);
                    }
                  }}
                />
              ) : null}
            </div>

            {hasGuessed && roundScore !== null && !gameOver && (
              <Results
                onNextImage={() => {
                  handleNextImageFromResults();
                }}
                score={roundScore}
              />
            )}
          </div>

          {/* GameOver Stats*/}
          {gameOver && (
            <div className="items-center justify-center bg-gray-600/60 border-white h-32 w-64 rounded-md">
              <div className="mt-2 text-white text-xl">
                <GameOver finalScore={finalScore} />
              </div>
            </div>
          )}

          {/* iframe map in corner */}
          <div
            className="absolute bottom-2 right-2 transition-all duration-300 ease-in-out"
            style={{
              height: isHovering ? "500px" : "325px",
              width: isHovering ? "500px" : "325px",
            }}
            onMouseEnter={() => setIsHovering(true)}
            onMouseLeave={() => setIsHovering(false)}
          >
            <iframe
              ref={iframeRef}
              src="/geoguess-map.html"
              style={{
                height: "100%",
                width: "100%",
                border: 0,
                borderRadius: 8,
              }}
              title="GeoGuesser Map"
              sandbox="allow-scripts allow-same-origin allow-forms"
            />
            {guessCoords && !hasGuessed && (
              <div
                style={{
                  position: "absolute",
                  bottom: "10%",
                  left: "47%",
                  transform: "translateX(-25%)",
                  fontSize: 22,
                  zIndex: 400,
                }}
              >
                <Guess
                  onGuess={() => {
                    submitGuess(guessCoords[0], guessCoords[1]);
                  }}
                  hasGuessed={hasGuessed}
                />
              </div>
            )}
          </div>
        </div>
      )}
    </div>
  );
}

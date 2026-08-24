import React, { useState } from "react";

interface TitleScreenProps {
  onStart: () => void;
  onCreateChallenge: () => void;
  onJoinChallenge: (input: string) => void;
  creatingChallenge?: boolean;
  joinError?: string | null;
}

const TitleScreen: React.FC<TitleScreenProps> = ({
  onStart,
  onCreateChallenge,
  onJoinChallenge,
  creatingChallenge = false,
  joinError = null,
}) => {
  const [joinCode, setJoinCode] = useState("");
  const [showJoin, setShowJoin] = useState(false);

  return (
    <div className="flex flex-col items-center justify-center min-h-screen bg-gradient-to-br from-yellow-300 to-blue-950 text-white text-center p-4">
      <h1 className="text-5xl font-extrabold mb-6 drop-shadow-xl">UCI GeoGuesser</h1>
      <p className="text-lg mb-8 max-w-md">
        Test your knowledge of UCI&apos;s geography! Click anywhere on the map to guess
        the location of a random image.
      </p>

      <div className="flex flex-col gap-3 w-full max-w-xs">
        <button
          onClick={onStart}
          className="bg-black text-white font-bold py-3 px-6 rounded-xl shadow-md hover:bg-blue-100 hover:text-black transition"
        >
          Play Solo
        </button>
        <button
          onClick={onCreateChallenge}
          disabled={creatingChallenge}
          className="bg-white/15 backdrop-blur text-white font-bold py-3 px-6 rounded-xl shadow-md border border-white/30 hover:bg-white/25 transition disabled:opacity-60"
        >
          {creatingChallenge ? "Creating challenge…" : "Challenge a Friend"}
        </button>
        <button
          onClick={() => setShowJoin((open) => !open)}
          className="text-white/90 font-semibold py-2 hover:text-white underline underline-offset-4"
        >
          Have a challenge link?
        </button>
      </div>

      {joinError && !showJoin && (
        <p className="mt-4 text-red-100 text-sm max-w-md">{joinError}</p>
      )}

      {showJoin && (
        <form
          className="mt-4 w-full max-w-md flex flex-col gap-2"
          onSubmit={(e) => {
            e.preventDefault();
            onJoinChallenge(joinCode);
          }}
        >
          <input
            value={joinCode}
            onChange={(e) => setJoinCode(e.target.value)}
            placeholder="Paste challenge link or ID"
            className="w-full rounded-xl px-4 py-3 text-black placeholder:text-gray-500"
          />
          {joinError && <p className="text-red-100 text-sm">{joinError}</p>}
          <button
            type="submit"
            className="bg-black text-white font-bold py-3 px-6 rounded-xl shadow-md hover:bg-blue-100 hover:text-black transition"
          >
            Join Challenge
          </button>
        </form>
      )}
    </div>
  );
};

export default TitleScreen;

--
-- PostgreSQL database dump (Corrected)
--

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET transaction_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';
SET default_table_access_method = heap;

-- Needed for gen_random_uuid() on Postgres < 13
CREATE EXTENSION IF NOT EXISTS pgcrypto WITH SCHEMA public;

-- Drop tables safely in reverse order of dependencies
DROP TABLE IF EXISTS public.challenge_attempts CASCADE;
DROP TABLE IF EXISTS public.challenge_images CASCADE;
DROP TABLE IF EXISTS public.challenges CASCADE;
DROP TABLE IF EXISTS public.players CASCADE;
DROP TABLE IF EXISTS public.player CASCADE;

-- 1. Challenges table (Generates the unique challenge)
-- No login required to create or play a challenge, so no player references here.
CREATE TABLE IF NOT EXISTS public.challenges (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(), -- Secure, unguessable ID for the link
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 2. Challenge Images table (Maps multiple images to one challenge)
CREATE TABLE IF NOT EXISTS public.challenge_images (
    id SERIAL PRIMARY KEY,
    challenge_id UUID REFERENCES public.challenges(id) ON DELETE CASCADE,
    image_url TEXT NOT NULL, -- Path to your S3/Cloud storage bucket
    latitude DOUBLE PRECISION NOT NULL,  -- answer coordinates, needed to score guesses
    longitude DOUBLE PRECISION NOT NULL,
    display_order INT NOT NULL, -- Ensures both players see images in the exact same order
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE (challenge_id, display_order)
);

-- 3. Challenge Attempts table
-- Anonymous, so there's no player identity to key off of. Instead, each challenge
-- can have exactly one 'creator' attempt and exactly one 'invitee' attempt, which
-- is enforced purely by the UNIQUE (challenge_id, role) constraint below —
-- this is what makes "you can only play a challenge link once" hold without login.
CREATE TABLE IF NOT EXISTS public.challenge_attempts (
    id SERIAL PRIMARY KEY,
    challenge_id UUID REFERENCES public.challenges(id) ON DELETE CASCADE,
    role VARCHAR(10) NOT NULL CHECK (role IN ('creator', 'invitee')),
    total_score INT,
    started_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    completed_at TIMESTAMP,
    UNIQUE (challenge_id, role) -- enforces one attempt per role per challenge
);

-- Indexes for instant lookups when a link is clicked / attempts are checked
CREATE INDEX IF NOT EXISTS idx_challenge_images_challenge_id ON public.challenge_images(challenge_id);
CREATE INDEX IF NOT EXISTS idx_challenge_attempts_challenge_id ON public.challenge_attempts(challenge_id);

--
-- PostgreSQL database dump complete
--
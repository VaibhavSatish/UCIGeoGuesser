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

-- Drop tables safely in reverse order of dependencies
DROP TABLE IF EXISTS public.challenge_images CASCADE;
DROP TABLE IF EXISTS public.challenges CASCADE;
DROP TABLE IF EXISTS public.players CASCADE;
DROP TABLE IF EXISTS public.player CASCADE;

-- 1. Users/Players table
CREATE TABLE IF NOT EXISTS public.players (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    email VARCHAR(255) UNIQUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 2. Challenges table (Generates the unique challenge)
CREATE TABLE IF NOT EXISTS public.challenges (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(), -- Secure, unguessable ID for the link
    creator_id INT REFERENCES public.players(id) ON DELETE SET NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 3. Challenge Images table (Maps multiple images to one challenge)
CREATE TABLE IF NOT EXISTS public.challenge_images (
    id SERIAL PRIMARY KEY,
    challenge_id UUID REFERENCES public.challenges(id) ON DELETE CASCADE,
    image_url TEXT NOT NULL, -- Path to your S3/Cloud storage bucket
    display_order INT NOT NULL, -- Ensures both players see images in the exact same order
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Index for instant image lookups when a link is clicked
CREATE INDEX IF NOT EXISTS idx_challenge_images_id ON public.challenge_images(challenge_id);

--
-- PostgreSQL database dump complete
--
--
-- PostgreSQL database dump
--

\restrict W31onZlWwg9scxnwLXsWAHcheLfgHT01c7wfFMZ2BV9KRcXoXYs6CDJ1Kp6uuUA

-- Dumped from database version 18.6 (Debian 18.6-1.pgdg13+2)
-- Dumped by pg_dump version 18.6 (Debian 18.6-1.pgdg13+2)
-- Runs only when database is created for the first time, to set up initial database structure

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

--
-- Name: pgcrypto; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS pgcrypto WITH SCHEMA public;


--
-- Name: EXTENSION pgcrypto; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON EXTENSION pgcrypto IS 'cryptographic functions';


SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: challenge_attempts; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.challenge_attempts (
    id integer NOT NULL,
    challenge_id uuid,
    role character varying(10) NOT NULL,
    total_score integer,
    started_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    completed_at timestamp without time zone,
    CONSTRAINT challenge_attempts_role_check CHECK (((role)::text = ANY ((ARRAY['creator'::character varying, 'invitee'::character varying])::text[])))
);


--
-- Name: challenge_attempts_id_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.challenge_attempts_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: challenge_attempts_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.challenge_attempts_id_seq OWNED BY public.challenge_attempts.id;


--
-- Name: challenge_images; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.challenge_images (
    id integer NOT NULL,
    challenge_id uuid,
    image_url text NOT NULL,
    latitude double precision NOT NULL,
    longitude double precision NOT NULL,
    display_order integer NOT NULL,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


--
-- Name: challenge_images_id_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.challenge_images_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: challenge_images_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.challenge_images_id_seq OWNED BY public.challenge_images.id;


--
-- Name: challenges; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.challenges (
    id uuid DEFAULT gen_random_uuid() NOT NULL,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


--
-- Name: images; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.images (
    id integer NOT NULL,
    gcs_url text NOT NULL,
    latitude double precision NOT NULL,
    longitude double precision NOT NULL
);


--
-- Name: images_id_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.images_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: images_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.images_id_seq OWNED BY public.images.id;


--
-- Name: challenge_attempts id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.challenge_attempts ALTER COLUMN id SET DEFAULT nextval('public.challenge_attempts_id_seq'::regclass);


--
-- Name: challenge_images id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.challenge_images ALTER COLUMN id SET DEFAULT nextval('public.challenge_images_id_seq'::regclass);


--
-- Name: images id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.images ALTER COLUMN id SET DEFAULT nextval('public.images_id_seq'::regclass);


--
-- Name: challenge_attempts challenge_attempts_challenge_id_role_key; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.challenge_attempts
    ADD CONSTRAINT challenge_attempts_challenge_id_role_key UNIQUE (challenge_id, role);


--
-- Name: challenge_attempts challenge_attempts_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.challenge_attempts
    ADD CONSTRAINT challenge_attempts_pkey PRIMARY KEY (id);


--
-- Name: challenge_images challenge_images_challenge_id_display_order_key; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.challenge_images
    ADD CONSTRAINT challenge_images_challenge_id_display_order_key UNIQUE (challenge_id, display_order);


--
-- Name: challenge_images challenge_images_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.challenge_images
    ADD CONSTRAINT challenge_images_pkey PRIMARY KEY (id);


--
-- Name: challenges challenges_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.challenges
    ADD CONSTRAINT challenges_pkey PRIMARY KEY (id);


--
-- Name: images images_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.images
    ADD CONSTRAINT images_pkey PRIMARY KEY (id);


--
-- Name: idx_challenge_attempts_challenge_id; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX idx_challenge_attempts_challenge_id ON public.challenge_attempts USING btree (challenge_id);


--
-- Name: idx_challenge_images_challenge_id; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX idx_challenge_images_challenge_id ON public.challenge_images USING btree (challenge_id);


--
-- Name: challenge_attempts challenge_attempts_challenge_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.challenge_attempts
    ADD CONSTRAINT challenge_attempts_challenge_id_fkey FOREIGN KEY (challenge_id) REFERENCES public.challenges(id) ON DELETE CASCADE;


--
-- Name: challenge_images challenge_images_challenge_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.challenge_images
    ADD CONSTRAINT challenge_images_challenge_id_fkey FOREIGN KEY (challenge_id) REFERENCES public.challenges(id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

\unrestrict W31onZlWwg9scxnwLXsWAHcheLfgHT01c7wfFMZ2BV9KRcXoXYs6CDJ1Kp6uuUA


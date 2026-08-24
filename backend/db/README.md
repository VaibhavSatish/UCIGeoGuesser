# PLS READ!!!

## Setting up DB

### For Mac Users
 
 DO THESE 4 STEPS ONLY ONCE
 `1. brew install postgresql@18`
 `2. brew services start postgresql@18`
 `3. psql postgres`
 `4. CREATE DATABASE db_name;`

### For Windows Users
WOMP WOMP

## Whenever you add Tables to the Database
RUN THE COMMAND BELOW
`pg_dump -U your_username -d db_name --schema-only --no-owner > schema.sql`

## If you Need to run the Schema file
RUN THE COMMAND BELOW
`psql -d db_name -f schema.sql`

## IMPORTANT! PLEASE FOLLOW THESE RULES
`1. CREATE TABLE IF NOT EXISTS table_name ...`

`2. ALTER TABLE table_name ADD COLUMN ...`





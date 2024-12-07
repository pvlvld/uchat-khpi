CREATE TYPE e_contact_status AS ENUM ('pending', 'accepted', 'rejected');
CREATE TYPE e_chat_type AS ENUM ('personal', 'group', 'channel');
CREATE TYPE e_media_type AS ENUM ('image', 'animation', 'audio', 'video');
CREATE TYPE e_chat_member_type AS ENUM ('member', 'admin', 'owner', 'left', 'banned', 'restricted');

CREATE TABLE media (
    media_id SERIAL PRIMARY KEY,
    media_type e_media_type NOT NULL,
    content BYTEA NOT NULL,
    duration INT DEFAULT NULL,
    height INT NOT NULL,
    width INT NOT NULL,
    -- metadata JSONB DEFAULT NULL, -- or metadata, flexible, can be resolution, duration, etc.
    hash_original VARCHAR(1024) NOT NULL,
    hash_compressed VARCHAR(1024) NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE users (
    user_id SERIAL PRIMARY KEY,
    username VARCHAR(32) UNIQUE NOT NULL,
    user_login VARCHAR(32) UNIQUE NOT NULL,
    password_hash VARCHAR(1024) NOT NULL,
    about VARCHAR(1024) DEFAULT NULL,
    locale VARCHAR(6) DEFAULT 'en',
    is_online BOOLEAN NOT NULL DEFAULT FALSE,
    profile_picture INT REFERENCES media(media_id) DEFAULT NULL,
    public_key VARCHAR(1024) NOT NULL,
    premium_until TIMESTAMP DEFAULT CURRENT_TIMESTAMP, 
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    deleted_at TIMESTAMP DEFAULT NULL
);

CREATE TABLE user_contacts (
    user_id INT REFERENCES users(user_id) ON DELETE CASCADE,
    contact_id INT REFERENCES users(user_id) ON DELETE CASCADE,
    status e_contact_status DEFAULT 'pending',
    PRIMARY KEY (user_id, contact_id)
);

CREATE TABLE chats (
    chat_id SERIAL PRIMARY KEY,
    chat_type e_chat_type NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE personal_chats (
    chat_id INT PRIMARY KEY REFERENCES chats(chat_id) ON DELETE CASCADE,
    user1_id INT REFERENCES users(user_id) ON DELETE CASCADE,
    user2_id INT REFERENCES users(user_id) ON DELETE CASCADE,
    backgroung INT REFERENCES media(media_id) DEFAULT NULL,
    UNIQUE (user1_id, user2_id)
);

CREATE TABLE group_chats (
    chat_id INT PRIMARY KEY REFERENCES chats(chat_id) ON DELETE CASCADE,
    group_name VARCHAR(120),
    about VARCHAR(1024) DEFAULT NULL,
    group_picture INT REFERENCES media(media_id) DEFAULT NULL,
    backgroung INT REFERENCES media(media_id) DEFAULT NULL,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);



CREATE TABLE messages (
    -- id SERIAL PRIMARY KEY, -- do we need it?
    message_id INT NOT NULL,
    chat_id INT REFERENCES chats(chat_id) ON DELETE CASCADE,
    sender_id INT REFERENCES users(user_id) ON DELETE SET NULL,
    message_text TEXT DEFAULT NULL,
    media INT REFERENCES media(media_id) DEFAULT NULL,
    reactions JSONB DEFAULT NULL,
    edited_at TIMESTAMP DEFAULT NULL,
    deleted_at TIMESTAMP DEFAULT NULL,
    reply_to_chat INT,
    reply_to_message INT,
    forwarded_from_chat INT,
    forwarded_from_message INT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (chat_id, message_id),
    FOREIGN KEY (reply_to_chat, reply_to_message) REFERENCES messages(chat_id, message_id) ON DELETE SET NULL,
    FOREIGN KEY (forwarded_from_chat, forwarded_from_message) REFERENCES messages(chat_id, message_id) ON DELETE SET NULL
);

CREATE INDEX idx_chat_messages ON messages (chat_id, timestamp);

-- Independent Message ID per Chat (telegram-like)
CREATE OR REPLACE FUNCTION increment_message_id()
RETURNS TRIGGER AS $$
DECLARE
    max_message_id INT;
BEGIN
    SELECT COALESCE(MAX(message_id), 0) + 1 INTO max_message_id
    FROM messages
    WHERE chat_id = NEW.chat_id;

    NEW.message_id := max_message_id;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Trigger for Messages Table
CREATE TRIGGER set_message_id
BEFORE INSERT ON messages
FOR EACH ROW
EXECUTE FUNCTION increment_message_id();
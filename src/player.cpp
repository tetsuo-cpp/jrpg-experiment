#include "player.h"

Player::Player(int tileX, int tileY, int tileSize, const std::string& spritePath)
    : m_tileX(tileX), m_tileY(tileY), m_tileSize(tileSize),
      m_sprite(spritePath.empty() ?
               Sprite(tileSize - 4, tileSize - 4, YELLOW) :
               Sprite(spritePath, tileSize, tileSize)),
      m_isMoving(false), m_targetX(tileX), m_targetY(tileY),
      m_moveProgress(0.0f) {
    m_pixelX = tileX * tileSize;
    m_pixelY = tileY * tileSize;
}

Player::~Player() {}

void Player::update(float deltaTime) {
    if (m_isMoving) {
        m_moveProgress += deltaTime * MOVE_SPEED;

        if (m_moveProgress >= 1.0f) {
            // Movement complete
            m_tileX = m_targetX;
            m_tileY = m_targetY;
            m_pixelX = m_tileX * m_tileSize;
            m_pixelY = m_tileY * m_tileSize;
            m_isMoving = false;
            m_moveProgress = 0.0f;
            m_sprite.setAnimating(false);
        } else {
            // Interpolate position
            int startX = (m_targetX - (m_targetX - m_tileX)) * m_tileSize;
            int startY = (m_targetY - (m_targetY - m_tileY)) * m_tileSize;
            int endX = m_targetX * m_tileSize;
            int endY = m_targetY * m_tileSize;

            m_pixelX = startX + (endX - startX) * m_moveProgress;
            m_pixelY = startY + (endY - startY) * m_moveProgress;
        }
    }

    m_sprite.update(deltaTime);
}

void Player::render(int cameraOffsetX, int cameraOffsetY) {
    int screenX = m_pixelX - cameraOffsetX + 2;
    int screenY = m_pixelY - cameraOffsetY + 2;
    m_sprite.render(screenX, screenY);
}

void Player::handleInput(const Tilemap& tilemap) {
    if (m_isMoving) {
        return; // Don't accept input while moving
    }

    int dx = 0, dy = 0;

    // Check keyboard input
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        dy = -1;
        m_sprite.setDirection(Direction::UP);
    } else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        dy = 1;
        m_sprite.setDirection(Direction::DOWN);
    } else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        dx = -1;
        m_sprite.setDirection(Direction::LEFT);
    } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        dx = 1;
        m_sprite.setDirection(Direction::RIGHT);
    }

    // Check gamepad input (if connected)
    if (IsGamepadAvailable(0)) {
        float axisX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        float axisY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

        if (axisY < -0.5f) {
            dy = -1;
            m_sprite.setDirection(Direction::UP);
        } else if (axisY > 0.5f) {
            dy = 1;
            m_sprite.setDirection(Direction::DOWN);
        } else if (axisX < -0.5f) {
            dx = -1;
            m_sprite.setDirection(Direction::LEFT);
        } else if (axisX > 0.5f) {
            dx = 1;
            m_sprite.setDirection(Direction::RIGHT);
        }

        // D-pad support
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
            dy = -1;
            m_sprite.setDirection(Direction::UP);
        } else if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
            dy = 1;
            m_sprite.setDirection(Direction::DOWN);
        } else if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
            dx = -1;
            m_sprite.setDirection(Direction::LEFT);
        } else if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
            dx = 1;
            m_sprite.setDirection(Direction::RIGHT);
        }
    }

    if (dx != 0 || dy != 0) {
        move(dx, dy, tilemap);
    }
}

void Player::move(int dx, int dy, const Tilemap& tilemap) {
    int newX = m_tileX + dx;
    int newY = m_tileY + dy;

    // Check if the target tile is walkable
    if (tilemap.isWalkable(newX, newY)) {
        m_targetX = newX;
        m_targetY = newY;
        m_isMoving = true;
        m_moveProgress = 0.0f;
        m_sprite.setAnimating(true);
    }
}

#include "game.hpp"

#include <GLFW/glfw3.h>
#include <time.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>


Game::Game() : window(800, 600, false, "Weird Tetris"), grid(gridWidth * gridHeight), ticks(defTicks), tick(ticks), score(0), losses(0) {
  this->running = true;
}

void Game::update() {
  running = running && this->window.getKey(GLFW_KEY_ESCAPE) == GLFW_RELEASE;

  tick--;
  if(tick == 0 || (this->window.getKey(GLFW_KEY_DOWN) != GLFW_RELEASE && !prevDown)) {
    tick = ticks;
    
    bool canmove = true;

    const int *p = this->piece.getArray();

    posy++;

    for(int y = 0; y < 4 && canmove; ++y)
      for(int x = 0; x < 4 && canmove; ++x) {
        if(p[y * 4 + x] != 0 && posy + y >= 0)
          canmove = canmove && (posy + y < gridHeight) && (this->grid[(posy + y) * gridWidth + posx + x] == 0);
      }

    if(canmove)
      passGridBuffer();
    else {
      posy--;
      if(!moved) {
        for(int y = 3; y >= 0; --y) {
          for(int x = 0; x < 4; ++x) {
            if(p[y * 4 + x] != 0 && posy + y < 0) {
              losses++;
              newGame();
              prevLeft = this->window.getKey(GLFW_KEY_LEFT) != GLFW_RELEASE;
              prevRight = this->window.getKey(GLFW_KEY_RIGHT) != GLFW_RELEASE;
              prevUp = this->window.getKey(GLFW_KEY_UP) != GLFW_RELEASE;
              prevZ = this->window.getKey(GLFW_KEY_SEMICOLON) != GLFW_RELEASE;
              prevDown = this->window.getKey(GLFW_KEY_DOWN) != GLFW_RELEASE;
              prevSpace = this->window.getKey(GLFW_KEY_SPACE) != GLFW_RELEASE;
              prevShift = this->window.getKey(GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE;
              return;
            }
            if(p[y * 4 + x] != 0)
              this->grid[(posy + y) * gridWidth + posx + x] = p[y * 4 + x];
          }
        }
        for(int y = posy + 3; y >= 1; --y) {
          bool line = true;
          for(int x = 0; x < gridWidth && line; ++x)
            line = line && (this->grid[y * gridWidth + x] != 0);
          if(line) {
            for(int ly = y - 1; ly >= 0; --ly)
              for(int x = 0; x < gridWidth; ++x)
                this->grid[(ly + 1) * gridWidth + x] = this->grid[ly * gridWidth + x];
            score++;
            ticks-=2;
            if(ticks < 1)
              ticks = 1;
            y++;
          }
        }
        held = false;
        choosePiece();
      }
      moved = false;
    }
  }
  if(this->window.getKey(GLFW_KEY_SPACE) != GLFW_RELEASE && !prevSpace) {
    bool canmove = true;

    const int *p = this->piece.getArray();

    while(canmove) {

      posy++;

      for(int y = 3; y >= 0 && canmove; --y)
        for(int x = 0; x < 4 && canmove; ++x) {
          if(p[y * 4 + x] != 0 && posy + y >= 0)
            canmove = canmove && (posy + y < gridHeight) && (this->grid[(posy + y) * gridWidth + posx + x] == 0);
        }
    }

    posy--;
    for(int y = 0; y < 4; ++y) {
      for(int x = 0; x < 4; ++x) {
        if(p[y * 4 + x] != 0 && posy + y < 0) {
          losses++;
          newGame();
          prevLeft = this->window.getKey(GLFW_KEY_LEFT) != GLFW_RELEASE;
          prevRight = this->window.getKey(GLFW_KEY_RIGHT) != GLFW_RELEASE;
          prevUp = this->window.getKey(GLFW_KEY_UP) != GLFW_RELEASE;
          prevZ = this->window.getKey(GLFW_KEY_SEMICOLON) != GLFW_RELEASE;
          prevDown = this->window.getKey(GLFW_KEY_DOWN) != GLFW_RELEASE;
          prevSpace = this->window.getKey(GLFW_KEY_SPACE) != GLFW_RELEASE;
          prevShift = this->window.getKey(GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE;
          return;
        }
        if(p[y * 4 + x] != 0)
          this->grid[(posy + y) * gridWidth + posx + x] = p[y * 4 + x];
      }
    }
    for(int y = posy + 3; y >= 1; --y) {
      bool line = true;
      for(int x = 0; x < gridWidth && line; ++x)
        line = line && (this->grid[y * gridWidth + x] != 0);
      if(line) {
        for(int ly = y - 1; ly >= 0; --ly) {
          for(int x = 0; x < gridWidth; ++x)
            this->grid[(ly + 1) * gridWidth + x] = this->grid[ly * gridWidth + x];
        }
        y++;
        ticks-=2;
        if(ticks < 1)
          ticks = 1;
        score++;
      }
    }
    held = false;
    choosePiece();
  }

  if(this->window.getKey(GLFW_KEY_LEFT) != GLFW_RELEASE && !prevLeft) {
    bool canmove = true;

    const int *p = this->piece.getArray();

    posx--;

    for(int y = 0; y < 4 && canmove; ++y)
      for(int x = 0; x < 4 && canmove; ++x) {
        if(p[y * 4 + x] != 0 && posy + y >= 0)
          canmove = canmove && (posx + x >= 0) && (posy + y < 0 || this->grid[(posy + y) * gridWidth + posx + x] == 0);
      }

    moved = canmove;

    if(canmove)
      passGridBuffer();
    else
      posx++;
  }
  else if(this->window.getKey(GLFW_KEY_RIGHT) != GLFW_RELEASE && !prevRight) {
    bool canmove = true;

    const int *p = this->piece.getArray();

    posx++;

    for(int y = 0; y < 4 && canmove; ++y)
      for(int x = 0; x < 4 && canmove; ++x) {
        if(p[y * 4 + x] != 0)
          canmove = canmove && (posx + x < gridWidth) && (posy + y < 0 || this->grid[(posy + y) * gridWidth + posx + x] == 0);
      }

    moved = canmove;

    if(canmove)
      passGridBuffer();
    else
      posx--;
  }
  else if(this->window.getKey(GLFW_KEY_UP) != GLFW_RELEASE && !prevUp) {
    Mino newMino(this->piece);

    newMino.rotClock();

    const int *p = newMino.getArray();

    bool canrotate = true;

    for(int y = 0; y < 4 && canrotate; ++y)
      for(int x = 0; x < 4 && canrotate; ++x) {
        if(p[y * 4 + x] != 0)
          canrotate = canrotate && (posx + x < gridWidth) && (posx + x >= 0) && (posy + y < 0 || this->grid[(posy + y) * gridWidth + posx + x] == 0);
      }

    moved = canrotate;

    if(canrotate) {
      this->piece = newMino;
      passGridBuffer();
    }
  }
  else if(this->window.getKey(GLFW_KEY_SEMICOLON) != GLFW_RELEASE && !prevZ) {
    Mino newMino(this->piece);

    newMino.rotCounterClock();

    const int *p = newMino.getArray();

    bool canrotate = true;

    for(int y = 0; y < 4 && canrotate; ++y)
      for(int x = 0; x < 4 && canrotate; ++x) {
        if(p[y * 4 + x] != 0)
          canrotate = canrotate && (posx + x < gridWidth) && (posx + x >= 0) && (this->grid[(posy + y) * gridWidth + posx + x] == 0);
      }

    moved = canrotate;

    if(canrotate) {
      this->piece = newMino;
      passGridBuffer();
    }
  }
  
  if(this->window.getKey(GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE && !prevShift && !held) {
    held = true;

    Mino newMino(this->piece);
    this->piece = this->hold;
    this->hold = newMino;

    this->posx = 3;
    this->posy = this->piece.startOffset();

    passGridBuffer();
  }

  prevLeft = this->window.getKey(GLFW_KEY_LEFT) != GLFW_RELEASE;
  prevRight = this->window.getKey(GLFW_KEY_RIGHT) != GLFW_RELEASE;
  prevUp = this->window.getKey(GLFW_KEY_UP) != GLFW_RELEASE;
  prevZ = this->window.getKey(GLFW_KEY_SEMICOLON) != GLFW_RELEASE;
  prevDown = this->window.getKey(GLFW_KEY_DOWN) != GLFW_RELEASE;
  prevSpace = this->window.getKey(GLFW_KEY_SPACE) != GLFW_RELEASE;
  prevShift = this->window.getKey(GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE;
}

void Game::newGame() {
  for(int i = 0; i < gridWidth * gridHeight; ++i)
    this->grid[i] = 0;

  score = 0;
  ticks = defTicks;

  choosePiece();
}

void Game::draw() {
  this->window.clearScreen();

  this->p.use();
  this->vao.bind();
  this->inds.drawInstanced(gridWidth * gridHeight + 16 * 2);
  this->vao.unbind();

  this->text.use();
  scoreText.draw();
  lossesText.draw();
}

void Game::init() {
  this->window.clearColor(glm::vec3(0.2f, 0.2f, 0.2f));

  FragmentShader f("res/texture.frag");
  VertexShader v("res/texture.vert");
  FragmentShader ffrag("res/text.frag");
  VertexShader fvert("res/text.vert");

  this->p = ShaderProgram(f, v);
  this->text = ShaderProgram(ffrag, fvert);

  this->t = Texture("res/tile.png");
  this->p["tex"] = this->t;
  this->p["P"] = glm::ortho(0.0f, (float)this->window.width, (float)this->window.height, 0.0f, -0.1f, 100.0f);
  this->p["V"] = glm::mat4(1.0f);
  this->p["M"] = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(this->window.width / 2 - (gridWidth * 25) / 2, this->window.height / 2 - (gridHeight * 25) / 2, 0.0f)), glm::vec3(25.0f, 25.0f, 0.0f));


  this->font = Texture("res/font.png", GL_NEAREST);
  this->text["tex"] = this->font;
  this->text["P"] = glm::ortho(0.0f, (float)this->window.width, (float)this->window.height, 0.0f, -0.1f, 100.0f);
  this->text["V"] = glm::mat4(1.0f);
  this->text["M"] = glm::mat4(1.0f);

  std::vector<glm::vec3> verts;
  verts.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
  verts.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
  verts.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
  verts.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
  vertices = Buffer(verts);
  vao.setAttrib(this->p, "vertexPosition", vertices, 3, GL_FLOAT, false);

  std::vector<glm::vec2> temp_uvs;
  temp_uvs.push_back(glm::vec2(0, 1));
  temp_uvs.push_back(glm::vec2(0, 0));
  temp_uvs.push_back(glm::vec2(1, 0));
  temp_uvs.push_back(glm::vec2(1, 1));
  uvs = Buffer(temp_uvs);
  vao.setAttrib(this->p, "vertexUV", uvs, 2, GL_FLOAT, false);

  std::vector<unsigned> indices;
  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(2);
  indices.push_back(2);
  indices.push_back(3);
  indices.push_back(0);

  inds = Buffer(indices, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);

  std::vector<glm::vec2> poss;
  for(int y = 0; y < gridHeight; ++y)
    for(int x = 0; x < gridWidth; ++x)
      poss.push_back(glm::vec2((float)x, (float)y));

  for(int y = 0; y < 4; ++y)
    for(int x = 0; x < 4; ++x)
      poss.push_back(glm::vec2(-6.0f + (float)x, (float)y));

  for(int y = 0; y < 4; ++y)
    for(int x = 0; x < 4; ++x)
      poss.push_back(glm::vec2(gridWidth + 1 + (float)x, (float)y));

  positions = Buffer(poss);
  vao.setAttrib(this->p, "tilePosition", positions, 2, GL_FLOAT, false, 1);
  this->vao.unbind();

  newGame();
}

void Game::choosePiece() {
  this->piece = this->next;
  this->next = Mino();
  posx = 3;
  posy = this->piece.startOffset();

  passGridBuffer();
}

void Game::passGridBuffer() {
  std::vector<glm::vec3> gridColors(gridWidth * gridHeight + 16 * 2);
  for(int i = 0; i < gridWidth * gridHeight; ++i)
      gridColors[i] = Game::pieceColors[this->grid[i]];

  const int *p = piece.getArray();

  bool canmove = true;

  int ghostposy = posy;

  while(canmove) {
    ghostposy++;

    for(int y = 3; y >= 0 && canmove; --y)
      for(int x = 0; x < 4 && canmove; ++x) {
        if(p[y * 4 + x] != 0 && ghostposy + y >= 0)
          canmove = canmove && (ghostposy + y < gridHeight) && (this->grid[(ghostposy + y) * gridWidth + posx + x] == 0);
      }
  }

  ghostposy--;

  for(int y = 0; y < 4; ++y) {
      for(int x = 0; x < 4; ++x) {
        if(ghostposy + y >= 0 && p[y * 4 + x] != 0 && this->grid[(ghostposy + y) * gridWidth + posx + x] <= 1)
          gridColors[(ghostposy + y) * gridWidth + (posx + x)] = pieceColors[1];
      }
    }

  for(int y = 0; y < 4; ++y) {
    bool clear = true;

    for(int x = 0; x < gridWidth && clear; ++x) {
      clear = clear && (this->grid[(ghostposy + y) * gridWidth + x] != 0 || (x >= posx && x < posx + 4 && p[y * 4 + (x - posx)] != 0));
    }

    if(clear) {
      for(int x = 0; x < gridWidth; ++x) {
        glm::vec3 &color = gridColors[(ghostposy + y) * gridWidth + x];
        color.r *= .6f;
        color.g *= .6f;
        color.b *= .6f;
      }
    }
  }

  for(int y = 0; y < 4; ++y) {
      for(int x = 0; x < 4; ++x) {
        if(posy + y >= 0 && p[y * 4 + x] != 0 && this->grid[(posy + y) * gridWidth + posx + x] <= 1)
          gridColors[(posy + y) * gridWidth + (posx + x)] = pieceColors[p[y * 4 + x]];
      }
    }

  const int *h = this->hold.getArray();

  for(int i = 0; i < 16; ++i)
    gridColors[gridWidth * gridHeight + i] = pieceColors[h[i]];

  const int *n = this->next.getArray();

  for(int i = 0; i < 16; ++i)
    gridColors[gridWidth * gridHeight + 16 + i] = pieceColors[n[i]];

  gridBuffer.loadData(gridColors, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
  vao.setAttrib(this->p, "tileColor", gridBuffer, 3, GL_FLOAT, false, 1);

  std::stringstream scoreStream;
  scoreStream << "Score - ";
  scoreStream << this->score;

  scoreText = Text(text, scoreStream.str(), 3, 3, 20.0f);

  std::stringstream lossesStream;
  lossesStream << "Losses - ";
  lossesStream << this->losses;
  lossesText = Text(text, lossesStream.str(), this->window.width - lossesStream.str().length() * 20.0f - 3.0f, 3.0f, 20.0f);
}

void Game::run() {
  while(this->running && !this->window.shouldClose()) {
    update();
    draw();
    this->window.swapBuffers();
    glfwPollEvents();
  }
}

void Game::cleanup() {
  glfwTerminate();
}

const glm::vec3 Game::pieceColors[Mino::numPieces + 2] = {
  glm::vec3(1.0f, 1.0f, 1.0f),
  glm::vec3(0.7f, 0.7f, 0.7f),
  glm::vec3(0.3f, 1.0f, 1.0f),
  glm::vec3(0.3f, 1.0f, 0.3f),
  glm::vec3(1.0f, 0.3f, 0.3f),
  glm::vec3(0.3f, 0.3f, 1.0f),
  glm::vec3(1.0f, 0.6f, 0.3f),
  glm::vec3(1.0f, 1.0f, 0.3f),
  glm::vec3(0.6f, 0.3f, 0.6f),

};
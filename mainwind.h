/****************************************************************************
**
**
**    Hex Wars - Copyright (C) 2017 Tomasz Ciborski
**
**    This file is part of Hex Wars.
**
**    Hex Wars is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    Hex Wars is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with Hex Wars.  If not, see <http://www.gnu.org/licenses/>.
**
**
****************************************************************************/
/****************************************************************************
 *
 *  mainwind.h
 *
 *  the most crucial file of the project: MainWind class definition. Contains
 *  ALL of the functions, routines, variables, included classes that make the
 *  whole game complete. I decided to split them into sections, I hope the file
 *  is readable.
 *
 * *************************************************************************/

#ifndef MAINWIND_H
#define MAINWIND_H

#include <QWidget>
#include <QMainWindow>

#include "./ui/hexitem.h"
#include "./ui/strings.h"
#include "./ui/dialog.h"
#include "./ui/sound.h"
#include "./game/piecerules.h"
#include "struct.h"
#include "const.h"
#include "rand.h"


QT_BEGIN_NAMESPACE
class QAction;
class QPoint;
class QToolButton;
class QLabel;
class QDockWidget;
class QTextStream;
QT_END_NAMESPACE

class MainWind : public QMainWindow{
    Q_OBJECT

public:
    MainWind();
    QGraphicsScene *minimapScene;
    QGraphicsPixmapItem *minimapBackground;
    QGraphicsLineItem *minimapVertP, *minimapHoriP;
    Random R;
    Hexagon *hexagon;


protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void doNextUnit();
    void doPrevUnit();
    void doCenterCameraAt();
    void doDeselect();
    void doCityCloseUp();
    void doLandParas();
    void doDelete();
    void doSurrender();
    void doEndTurn();

    void doUpdateProdLabel();
    void doSetCityProduction();

    void doSetDefaultRules();
    void doUpdateNewGameRules();
    void doShowNewGameWindow();
    void doUpdateDiffLabel();
    void doShowRulesEdit();
    void doShowRankings();
    void doShowHighScores();
    void doShowLog();
    void doShowGoto();
    void doGotoHex();
    void doNewGame();
    void doLoadGame();
    void doSaveGame();
    void doShowGameInfo();
    void doClose();
    void doVisitWWW();
    void doManual();

    void doUpdateBorderPen();

private:

    /* Embedded classes (windows, string sets) */
    Strings str;
    CityCloseUp cityWindow;
    NewGame newGameWindow;
    EditRules rulesWindow;
    Rankings rankingWindow;
    Highscores highscoreWindow;
    Programlog logWindow;
    GotoHex gotoWindow;
    PieceRules rule, tmpRule;
    AttackRules attack;
    ProdRules prodRatio;
    QFileDialog *fileDialog;
    QMessageBox ingameQuestion;
    QMessageBox *ingameInfo;
    Sound sound;

    void LOG(u_int8_t type, QString strng);
    void createWindows();
    void delay();
    void createRanking(u_int8_t type, u_int8_t *table);

    /* Core functions on hexes */
    void createHexagonPath();
    bool hexExists(int i);
    QPoint hexPosition(int base);
    QPoint hexRelativeCoords(int base, int target);
    double distanceBetweenHexes(int a, int b);
    int hexWhichDirection(int base, int p);
    void gaussianHeight(double A, int base, double sigmaX, double sigmaY);
    double sizeInHexes(double howMany);
    void setHexVisible(int i, u_int8_t player);
    void setHexInvisible(int i, u_int8_t player);
    void setHexUncovered(int i, u_int8_t player);
    bool isShore(int i);
    int findShore(int pos);
    bool isHexUncovered(int i, u_int8_t player);
    bool isHexVisible(int i, u_int8_t player);
    bool isContigToAlly(int i, u_int8_t player);
    bool isContainingAlly(int i, u_int8_t player);
    bool hasFriendlyCity(int i, u_int8_t player);
    bool hasFriendlyShip(int i, u_int8_t player, u_int8_t type);
    bool isContigToFriendlyCity(int i, u_int8_t player);
    bool hasEnemyUnit(int i, u_int8_t player);
    bool isInFriendlyControl(int i, u_int8_t player);
    void putShipOnTop(int i, u_int8_t type);
    void unitDisappear(int hex, int depth, u_int8_t player);
    void unitAppear(int hex, int depth, u_int8_t player);
    QQueue<int> tempQ;

    /* Graphics */
    QImage *texture;
    QImage *unitImage;
    QImage *digit;
    QRgb *regionColor;
    void createTextures();
    void paintTerrain();
    void paintHex(int i);
    QImage sumImages(QImage *a, QImage *b);
    QImage paintUnit(QImage *hexT, QImage *unitT);
    QImage paintMoreUnits(QImage *hexT, QImage *digit);
    QImage paintTransported(QImage *hexT, QImage *digit);
    QImage paintHPBar(QImage *hexT, int health);
    void updateHex(int i);

    /* Menus */
    QAction *menuAction[12], *option[3];
    QMenu *menu[4];
    void createMenus();
    void loadOptions();


    /* Left-side dock */
    Minimap minimap;
    QLabel *dockImage[4], *titleDock[3], *unitData[3];
    QPushButton *panelAction[8];
    QGraphicsView *minimapView;
    QPixmap chart;
    void createDock();
    void drawMinimapHex(int hex);
    void updateMinimap();
    void updateMinimapPointer();
    void drawMinimapAllHexes();
    void updateSelection();
    void updateNowPlaying();
    void drawDockChart();
    void createMinimap();
    void paintMinimap(int i);

    /* Game variables */
    int SIZEOF_MAP, rows, cols, tAmount, pixelWidth, curSelection;
    int NOF_ISLANDS, NOF_REGIONS;
    u_int8_t gdiff, GAME_VERSION;
    QVector<int> islandSize, islandCityCount;
    QVector<ScanCounts> scanCounts;
    int curPlayer, lastHumanPlayer, humansInGame, playersInGame;
    unsigned int gseed, curTurn;
    Player player[6];
    QVector<City> city;

    /* High scores */
    QVector<HighScoreItem> highscore;
    void clearScores();
    void highScoreLoad();
    void highScoreSave(u_int8_t a);
    void updateHScoresWindow();

    /* World generation */
    void generateIsland(int size, int landAmount);
    void logicalIslandGen();
    void logicalRegionGen();
    void spawnCities();
    void spawnStartingPos();


    /* Movement and pathfinding */
    QMap<int, int> cameFrom;
    QVector<int> closedSet;
    QVector<int> openSet;
    QVector<int> movePath;
    QVector<int> findPath(int start, int goal, u_int8_t mode);
    QVector<int> reconstructPath(int current);
    int moveUnit(int start, int goal);
    /* There's no priority queue in Qt, so I'll use my own
     * functions. They will be used in pathfinding only */
    void PQinsert(QVector<int> &Q, int i);
    int PQfront(QVector<int> &Q);
    void PQheapify(QVector<int> &Q, int i);
    void execSingleMove(int from, int to);
    void captureCity(int cityId, u_int8_t actor);
    bool canLandParas(int hex);
    void landParas(int hex);
    bool spawnUnit(int target, Unit toSpawn);
    bool canAllocateNewUnit(int target, u_int8_t type, u_int8_t owner);
    void destroyUnit(int hex, u_int8_t actor);
    void attackUnit(int from, int at);
    Unit newUnit(u_int8_t type, u_int8_t owner);

    /* Game organization - turns */
    void endTurn();
    void resetDiscoveredMap();
    void beginTurn();
    void endGame(int how);

    /* Artificial Intelligence functions */
    unsigned int transportReqId;
    u_int16_t *allyUnit, *enemyUnit, **transList, **carrierList;
    QVector<AiCity> cityData;
    void shuffleList(u_int16_t *list, int size);
    float prodFactor(u_int8_t i);
    bool overproduction(int where, int ratio, int *cityCount);
    void setCompProduction(int where, u_int8_t type, int *cityCount);
    void compProduce(int where, int *cityCount, int transSize);
    void compProdAllCities(int *cityCount,
                  int allySize, int enemySize, int transSize);
    void countObjects(int allySize, int enemySize);
    void compMove();
    void compManageTransport(int i, int allySize);
    int transportMove(int pos, int allySize);
    int unloadUnitNearby(int pos, u_int8_t owner);
    int findSomethingToKill(int pos, int allySize,
                            int enemySize, int transSize, int maxRange);
    int findJobCompSingleUnit(int pos,int allySize, int enemySize, int transSize, int carrierSize);
    void assessDanger(int enemySize);
    void compSwapUnit(int pos, int a);
    int avgAttackVal(u_int8_t type, bool isCity);
    int killDesire(int benefit, int attac, int loss, int vuln, int victimCount);
    int bomberKillDesire(int pos, int target);
    int findNearestSafeCity(int pos, Unit *u);
    int findNearestFuelReplenish(int pos, int carrierSize);
    int parasMove(int pos);
    int maxNavalMoves(Unit *u);
    bool findFirstUndone(int hex);
    void countPlayerUnits();


    /* MainWind graphics view manipulation */
    QPen borderPen;
    void createMainWindTitle();
    int curHex, destHex, curCity;
    bool dragMode;
    void selectNothingEffect();
    void selectUnitEffect();
    void selectEmptyCityEffect();
    void moveCameraTo(int hex);
    int updateButtonGeometry(QToolButton *button, int x, int y);
    void createHexItem(const QPainterPath &path, const QPoint &pos);
    int itemAt(const QPoint &pos);
    void moveItemTo(const QPoint &pos);
    QPoint cameraPos;
    QPoint initialItemPosition(const QPainterPath &path);

    QList<HexItem> shapeItems;
    QPainterPath hexagonPath;
    QPolygon hexPolygon;

    QPoint previousPosition;
    HexItem *itemInMotion;

};

#endif

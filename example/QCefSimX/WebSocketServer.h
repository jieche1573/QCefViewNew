#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QtCore/QMutex>
#include <memory>
#include <mutex>

class QWebSocketServer;
class WebSocketChannel;

 class WebSocketServer : public QObject
{

  Q_OBJECT

public:
  static WebSocketServer& Instance()
  {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() { _instance.reset(new WebSocketServer); });

    return *_instance;
  }
  ~WebSocketServer() override;

private:
  static std::unique_ptr<WebSocketServer> _instance;
  static std::mutex _mutex;

private:
  WebSocketServer(QObject* parent = nullptr);

  //fly --start--
signals:
    void queryFly(QString json);
    void queryFlyAdd(QString json);
    void queryFlyDel(bool ok);
    void queryFlyEdit(bool ok);
    void queryFlySearch(QString json);
    void queryFlyById(QString json);
  public slots:
    void queryFlyReq();
    void queryFlyAddReq(QString json);
    void queryFlyDelReq(QString id);
    void queryFlyEditReq(QString json);
    void queryFlySearchReq(QString KeyWord);
    void queryFlyByIdReq(QString id);

//fly --end--

  //equipment --start--
signals:
    void queryEquipment(QString json);
    void queryEquipmentAdd(QString json);
    void queryEquipmentDel(bool ok);
    void queryEquipmentEdit(bool ok);
    void queryEquipmentSearch(QString json);
    void queryEquipmentById(QString json);
  public slots:
    void queryEquipmentReq();
    void queryEquipmentAddReq(QString json);
    void queryEquipmentDelReq(QString id);
    void queryEquipmentEditReq(QString json);
    void queryEquipmentSearchReq(QString KeyWord);
    void queryEquipmentByIdReq(QString id);

//equipment --end--

  //position --start--
signals:
    void queryPosition(QString json);
    void queryPositionAdd(QString json);
    void queryPositionDel(bool ok);
    void queryPositionEdit(bool ok);
    void queryPositionSearch(QString json);
  public slots:
    void queryPositionReq();
    void queryPositionAddReq(QString json);
    void queryPositionDelReq(QString id);
    void queryPositionEditReq(QString json);
    void queryPositionSearchReq(QString KeyWord);

//position --end--

  //spare_part --start--
signals:
    void querySparePart(QString json);
    void querySparePartAdd(QString json);
    void querySparePartDel(bool ok);
    void querySparePartEdit(bool ok);
    void querySparePartSearch(QString json);
  public slots:
    void querySparePartReq();
    void querySparePartAddReq(QString json);
    void querySparePartDelReq(QString id);
    void querySparePartEditReq(QString json);
    void querySparePartSearchReq(QString KeyWord);

//spare_part --end--

  //position_skill --start--
signals:
    void queryPositionSkill(QString json);
    void queryPositionSkillAdd(QString json);
    void queryPositionSkillDel(bool ok);
    void queryPositionSkillEdit(bool ok);
    void queryPositionSkillByPosition(QString json);
    void queryPositionSkillByAircraft(QString json);
    void queryPositionSkillBatchAdd(QString json);
  public slots:
    void queryPositionSkillReq();
    void queryPositionSkillAddReq(QString json);
    void queryPositionSkillDelReq(QString id);
    void queryPositionSkillEditReq(QString json);
    void queryPositionSkillByPositionReq(QString gwId);
    void queryPositionSkillByAircraftReq(QString jxId);
    void queryPositionSkillBatchAddReq(QString json);
    void queryPositionSkillDelByPositionReq(QString gwId);

//position_skill --end--

  //process --start--
signals:
    void queryProcess(QString json);
    void queryProcessAdd(QString json);
    void queryProcessDel(bool ok);
    void queryProcessEdit(bool ok);
    void queryProcessSearch(QString json);
    void queryProcessByModelId(QString json);
    void queryProcessByNode(QString json);
    void queryProcessByLevel(QString json);
    void queryProcessByPersonId(QString json);
    void queryProcessByCarId(QString json);
  public slots:
    void queryProcessReq();
    void queryProcessAddReq(QString json);
    void queryProcessDelReq(QString id);
    void queryProcessEditReq(QString json);
    void queryProcessSearchReq(QString KeyWord);
    void queryProcessByModelIdReq(QString modelId);
    void queryProcessByNodeReq(QString node);
    void queryProcessByLevelReq(QString level);
    void queryProcessByPersonIdReq(QString personId);
    void queryProcessByCarIdReq(QString carId);

//process --end--

  //scenario --start--
signals:
    void queryScenario(QString json);
    void queryScenarioAdd(QString json);
    void queryScenarioDel(bool ok);
    void queryScenarioEdit(bool ok);
    void queryScenarioSearch(QString json);
    void queryScenarioById(QString json);
    void queryScenarioByStatus(QString json);
    void queryScenarioByAirport(QString json);
    void updateScenarioRateFields(bool ok);
  public slots:
    void queryScenarioReq();
    void queryScenarioAddReq(QString json);
    void queryScenarioDelReq(QString id);
    void queryScenarioEditReq(QString json);
    void queryScenarioSearchReq(QString KeyWord);
    void queryScenarioByIdReq(QString id);
    void queryScenarioByStatusReq(QString status);
    void queryScenarioByAirportReq(QString airport);
    void updateScenarioRateFieldsReq(QString json);

//scenario --end--

  //aircraft_entity --start--
signals:
    void queryAircraftEntity(QString json);
    void queryAircraftEntityAdd(QString json);
    void queryAircraftEntityDel(bool ok);
    void queryAircraftEntityEdit(bool ok);
    void queryAircraftEntityById(QString json);
    void queryAircraftEntityByCode(QString json);
    void queryAircraftEntityByScenarioId(QString json);
    void queryAircraftEntityByModelId(QString json);
    void queryAircraftEntityByLevel(QString json);
    void queryAircraftEntityByScenarioAndModel(QString json);
  public slots:
    void queryAircraftEntityReq();
    void queryAircraftEntityAddReq(QString json);
    void queryAircraftEntityDelReq(QString id);
    void queryAircraftEntityEditReq(QString json);
    void queryAircraftEntityByIdReq(QString id);
    void queryAircraftEntityByCodeReq(QString code);
    void queryAircraftEntityByScenarioIdReq(QString xdid);
    void queryAircraftEntityByModelIdReq(QString jxid);
    void queryAircraftEntityByLevelReq(QString bzdj);
    void queryAircraftEntityByScenarioAndModelReq(QString xdid, QString jxid);

//aircraft_entity --end--

  //flight_task --start--
signals:
    void queryFlightTask(QString json);
    void queryFlightTaskAdd(QString json);
    void queryFlightTaskDel(bool ok);
    void queryFlightTaskEdit(bool ok);
    void queryFlightTaskById(QString json);
    void queryFlightTaskByAircraftEntityId(QString json);
    void queryFlightTaskByTimeRange(QString json);
  public slots:
    void queryFlightTaskReq();
    void queryFlightTaskAddReq(QString json);
    void queryFlightTaskDelReq(QString id);
    void queryFlightTaskEditReq(QString json);
    void queryFlightTaskByIdReq(QString id);
    void queryFlightTaskByAircraftEntityIdReq(QString fjstid);
    void queryFlightTaskByTimeRangeReq(QString startTime, QString endTime);

//flight_task --end--


private:
  void requestAccessToken(void); //加锁//
  void releaseAccessToken(void); //解锁//
private:
  QMutex mutex; //多线程资源访问控制//
  QWebSocketServer* m_server = nullptr;
  WebSocketChannel* m_channel = nullptr;
};



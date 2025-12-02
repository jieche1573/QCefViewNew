#include "WebSocketServer.h"
#include "websocketchannel.h"

#include <mutex>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QSettings>
#include <QTimer>
#include <QWebSocketServer>

#include <AircraftModelTest.h>
#include "Equipment.h"
#include "Position.h"
#include "SparePart.h"
#include "PositionSkill.h"
#include "ProcessT.h"
#include "ScenarioModel.h"
#include "AircraftEntity.h"
#include "FlightTask.h"

std::unique_ptr<WebSocketServer> WebSocketServer::_instance;
std::mutex WebSocketServer::_mutex;

class QWebSocketServer;

WebSocketServer::WebSocketServer(QObject* parent)
  : QObject(parent)
{
  //建立QWebSocketServer，url是ws://localhost:12345
  m_server = new QWebSocketServer(QStringLiteral("QWebChannel Server"), QWebSocketServer::NonSecureMode);
#ifdef NDEBUG
  bool isListened = m_server->listen(QHostAddress::Any, 54321);
#else
  bool isListened = m_server->listen(QHostAddress::Any, 54321);
#endif

  if (!isListened) {
    qDebug() << "Failed to open web socket server.";
  }

  m_channel = new WebSocketChannel(m_server);
  m_channel->registerObject(QStringLiteral("qtClient"), this);
}

WebSocketServer::~WebSocketServer()
{
  m_channel->deregisterObject(this);
  delete m_channel;
  m_server->close();
  m_server->deleteLater();
}



void
WebSocketServer::requestAccessToken(void)
{
  mutex.lock(); //如果已经lock且还未unlock，一直等待，直到unlock，然后lock//
}

void
WebSocketServer::releaseAccessToken(void)
{
  mutex.unlock();
}


void
WebSocketServer::queryFlyReq()
{
  QTimer::singleShot(50, [this] {
      emit queryFly(AircraftModel::Instance().queryAllRecords());
      });
}

void WebSocketServer::queryFlyAddReq(QString json)
{

  QTimer::singleShot(50, [this, json] {
      emit queryFlyAdd(AircraftModel::Instance().addRecord(json));
    });
 
}

void
WebSocketServer::queryFlyDelReq(QString id)
{
  QTimer::singleShot(50, [this,id] {
      emit queryFlyDel(AircraftModel::Instance().deleteRecord(id));
      });
}

void
WebSocketServer::queryFlyEditReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryFlyEdit(AircraftModel::Instance().updateRecord(json));
      });
}

void
WebSocketServer::queryFlySearchReq(QString KeyWord)
{
    QTimer::singleShot(50, [this, KeyWord] {
        emit queryFlySearch(AircraftModel::Instance().queryByName(KeyWord));
        });
}

void
WebSocketServer::queryFlyByIdReq(QString id)
{
    QTimer::singleShot(50, [this, id] {
        emit queryFlyById(AircraftModel::Instance().queryRecordById(id));
        });
}

// Equipment implementation
void
WebSocketServer::queryEquipmentReq()
{
  QTimer::singleShot(50, [this] {
      emit queryEquipment(Equipment::Instance().queryAllRecords());
      });
}

void WebSocketServer::queryEquipmentAddReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryEquipmentAdd(Equipment::Instance().addRecord(json));
    });
}

void
WebSocketServer::queryEquipmentDelReq(QString id)
{
  QTimer::singleShot(50, [this, id] {
      emit queryEquipmentDel(Equipment::Instance().deleteRecord(id));
      });
}

void
WebSocketServer::queryEquipmentEditReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryEquipmentEdit(Equipment::Instance().updateRecord(json));
      });
}

void
WebSocketServer::queryEquipmentSearchReq(QString KeyWord)
{
    QTimer::singleShot(50, [this, KeyWord] {
        emit queryEquipmentSearch(Equipment::Instance().queryByName(KeyWord));
        });
}

void
WebSocketServer::queryEquipmentByIdReq(QString id)
{
    QTimer::singleShot(50, [this, id] {
        emit queryEquipmentById(Equipment::Instance().queryRecordById(id));
        });
}

// Position implementation
void
WebSocketServer::queryPositionReq()
{
  QTimer::singleShot(50, [this] {
      emit queryPosition(Position::Instance().queryAllRecords());
      });
}

void WebSocketServer::queryPositionAddReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryPositionAdd(Position::Instance().addRecord(json));
    });
}

void
WebSocketServer::queryPositionDelReq(QString id)
{
  QTimer::singleShot(50, [this, id] {
      emit queryPositionDel(Position::Instance().deleteRecord(id));
      });
}

void
WebSocketServer::queryPositionEditReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryPositionEdit(Position::Instance().updateRecord(json));
      });
}

void
WebSocketServer::queryPositionSearchReq(QString KeyWord)
{
    QTimer::singleShot(50, [this, KeyWord] {
        emit queryPositionSearch(Position::Instance().queryByName(KeyWord));
        });
}

// SparePart implementation
void
WebSocketServer::querySparePartReq()
{
  QTimer::singleShot(50, [this] {
      emit querySparePart(SparePart::Instance().queryAllRecords());
      });
}

void WebSocketServer::querySparePartAddReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit querySparePartAdd(SparePart::Instance().addRecord(json));
    });
}

void
WebSocketServer::querySparePartDelReq(QString id)
{
  QTimer::singleShot(50, [this, id] {
      emit querySparePartDel(SparePart::Instance().deleteRecord(id));
      });
}

void
WebSocketServer::querySparePartEditReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit querySparePartEdit(SparePart::Instance().updateRecord(json));
      });
}

void
WebSocketServer::querySparePartSearchReq(QString KeyWord)
{
    QTimer::singleShot(50, [this, KeyWord] {
        emit querySparePartSearch(SparePart::Instance().queryByName(KeyWord));
        });
}

// PositionSkill implementation
void
WebSocketServer::queryPositionSkillReq()
{
  QTimer::singleShot(50, [this] {
      emit queryPositionSkill(PositionSkill::Instance().queryAllRecords());
      });
}

void WebSocketServer::queryPositionSkillAddReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryPositionSkillAdd(PositionSkill::Instance().addRecord(json));
    });
}

void
WebSocketServer::queryPositionSkillDelReq(QString id)
{
  QTimer::singleShot(50, [this, id] {
      emit queryPositionSkillDel(PositionSkill::Instance().deleteRecord(id));
      });
}

void
WebSocketServer::queryPositionSkillEditReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryPositionSkillEdit(PositionSkill::Instance().updateRecord(json));
      });
}

void
WebSocketServer::queryPositionSkillByPositionReq(QString gwId)
{
    QTimer::singleShot(50, [this, gwId] {
        emit queryPositionSkillByPosition(PositionSkill::Instance().queryByPositionId(gwId));
        });
}

void
WebSocketServer::queryPositionSkillByAircraftReq(QString jxId)
{
    QTimer::singleShot(50, [this, jxId] {
        emit queryPositionSkillByAircraft(PositionSkill::Instance().queryByAircraftId(jxId));
        });
}

void
WebSocketServer::queryPositionSkillBatchAddReq(QString json)
{
    QTimer::singleShot(50, [this, json] {
        emit queryPositionSkillBatchAdd(PositionSkill::Instance().batchAddRecords(json));
        });
}

void
WebSocketServer::queryPositionSkillDelByPositionReq(QString gwId)
{
    QTimer::singleShot(50, [this, gwId] {
        emit queryPositionSkillDel(PositionSkill::Instance().deleteByPositionId(gwId));
        });
}

// Process implementation
void
WebSocketServer::queryProcessReq()
{
  QTimer::singleShot(50, [this] {
      emit queryProcess(ProcessT::Instance().queryAllRecords());
      });
}

void WebSocketServer::queryProcessAddReq(QString json)
{
  QTimer::singleShot(50, [this, json] { emit queryProcessAdd(ProcessT::Instance().addRecord(json));
    });
}

void
WebSocketServer::queryProcessDelReq(QString id)
{
  QTimer::singleShot(50, [this, id] { emit queryProcessDel(ProcessT::Instance().deleteRecord(id));
      });
}

void
WebSocketServer::queryProcessEditReq(QString json)
{
  QTimer::singleShot(50, [this, json] { emit queryProcessEdit(ProcessT::Instance().updateRecord(json));
      });
}

void
WebSocketServer::queryProcessSearchReq(QString KeyWord)
{
    QTimer::singleShot(50, [this, KeyWord] { emit queryProcessSearch(ProcessT::Instance().queryByName(KeyWord));
        });
}

void
WebSocketServer::queryProcessByModelIdReq(QString modelId)
{
    QTimer::singleShot(50, [this, modelId] { emit queryProcessByModelId(ProcessT::Instance().queryByModelId(modelId));
        });
}

void
WebSocketServer::queryProcessByNodeReq(QString node)
{
    QTimer::singleShot(50, [this, node] { emit queryProcessByNode(ProcessT::Instance().queryByNode(node));
        });
}

void
WebSocketServer::queryProcessByLevelReq(QString level)
{
    QTimer::singleShot(50, [this, level] { emit queryProcessByLevel(ProcessT::Instance().queryByLevel(level));
        });
}

void
WebSocketServer::queryProcessByPersonIdReq(QString personId)
{
    QTimer::singleShot(50, [this, personId] { emit queryProcessByPersonId(ProcessT::Instance().queryByPersonId(personId));
        });
}

void
WebSocketServer::queryProcessByCarIdReq(QString carId)
{
    QTimer::singleShot(50, [this, carId] { emit queryProcessByCarId(ProcessT::Instance().queryByCarId(carId));
        });
}

// Scenario implementation
void
WebSocketServer::queryScenarioReq()
{
  QTimer::singleShot(50, [this] {
      emit queryScenario(ScenarioModel::Instance().queryAllRecords());
      });
}

void WebSocketServer::queryScenarioAddReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryScenarioAdd(ScenarioModel::Instance().addRecord(json));
    });
}

void
WebSocketServer::queryScenarioDelReq(QString id)
{
  QTimer::singleShot(50, [this, id] {
      emit queryScenarioDel(ScenarioModel::Instance().deleteRecord(id));
      });
}

void
WebSocketServer::queryScenarioEditReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryScenarioEdit(ScenarioModel::Instance().updateRecord(json));
      });
}

void
WebSocketServer::queryScenarioSearchReq(QString KeyWord)
{
    QTimer::singleShot(50, [this, KeyWord] {
        emit queryScenarioSearch(ScenarioModel::Instance().queryByName(KeyWord));
        });
}

void
WebSocketServer::queryScenarioByIdReq(QString id)
{
    QTimer::singleShot(50, [this, id] {
        emit queryScenarioById(ScenarioModel::Instance().queryRecordById(id));
        });
}

void
WebSocketServer::queryScenarioByStatusReq(QString status)
{
    QTimer::singleShot(50, [this, status] {
        emit queryScenarioByStatus(ScenarioModel::Instance().queryByStatus(status));
        });
}

void
WebSocketServer::queryScenarioByAirportReq(QString airport)
{
    QTimer::singleShot(50, [this, airport] {
        emit queryScenarioByAirport(ScenarioModel::Instance().queryByAirport(airport));
        });
}

void
WebSocketServer::updateScenarioRateFieldsReq(QString json)
{
    QTimer::singleShot(50, [this, json] {
        emit updateScenarioRateFields(ScenarioModel::Instance().updateRateFields(json));
        });
}

void
WebSocketServer::updateScenarioPredictionReq(QString json)
{
    QTimer::singleShot(50, [this, json] {
        emit updateScenarioPrediction(ScenarioModel::Instance().updatePredictionField(json));
        });
}

// AircraftEntity implementation
void
WebSocketServer::queryAircraftEntityReq()
{
  QTimer::singleShot(50, [this] {
      emit queryAircraftEntity(AircraftEntity::Instance().queryAllRecords());
      });
}

void WebSocketServer::queryAircraftEntityAddReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryAircraftEntityAdd(AircraftEntity::Instance().addRecord(json));
    });
}

void
WebSocketServer::queryAircraftEntityDelReq(QString id)
{
  QTimer::singleShot(50, [this, id] {
      emit queryAircraftEntityDel(AircraftEntity::Instance().deleteRecord(id));
      });
}

void
WebSocketServer::queryAircraftEntityEditReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryAircraftEntityEdit(AircraftEntity::Instance().updateRecord(json));
      });
}

void
WebSocketServer::queryAircraftEntityByIdReq(QString id)
{
    QTimer::singleShot(50, [this, id] {
        emit queryAircraftEntityById(AircraftEntity::Instance().queryRecordById(id));
        });
}

void
WebSocketServer::queryAircraftEntityByCodeReq(QString code)
{
    QTimer::singleShot(50, [this, code] {
        emit queryAircraftEntityByCode(AircraftEntity::Instance().queryByCode(code));
        });
}

void
WebSocketServer::queryAircraftEntityByScenarioIdReq(QString xdid)
{
    QTimer::singleShot(50, [this, xdid] {
        emit queryAircraftEntityByScenarioId(AircraftEntity::Instance().queryByScenarioId(xdid));
        });
}

void
WebSocketServer::queryAircraftEntityByModelIdReq(QString jxid)
{
    QTimer::singleShot(50, [this, jxid] {
        emit queryAircraftEntityByModelId(AircraftEntity::Instance().queryByModelId(jxid));
        });
}

void
WebSocketServer::queryAircraftEntityByLevelReq(QString bzdj)
{
    QTimer::singleShot(50, [this, bzdj] {
        emit queryAircraftEntityByLevel(AircraftEntity::Instance().queryByLevel(bzdj));
        });
}

void
WebSocketServer::queryAircraftEntityByScenarioAndModelReq(QString xdid, QString jxid)
{
    QTimer::singleShot(50, [this, xdid, jxid] {
        emit queryAircraftEntityByScenarioAndModel(AircraftEntity::Instance().queryByScenarioAndModel(xdid, jxid));
        });
}

// FlightTask implementation
void
WebSocketServer::queryFlightTaskReq()
{
  QTimer::singleShot(50, [this] {
      emit queryFlightTask(FlightTask::Instance().queryAllRecords());
      });
}

void WebSocketServer::queryFlightTaskAddReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryFlightTaskAdd(FlightTask::Instance().addRecord(json));
    });
}

void
WebSocketServer::queryFlightTaskDelReq(QString id)
{
  QTimer::singleShot(50, [this, id] {
      emit queryFlightTaskDel(FlightTask::Instance().deleteRecord(id));
      });
}

void
WebSocketServer::queryFlightTaskEditReq(QString json)
{
  QTimer::singleShot(50, [this, json] {
      emit queryFlightTaskEdit(FlightTask::Instance().updateRecord(json));
      });
}

void
WebSocketServer::queryFlightTaskByIdReq(QString id)
{
    QTimer::singleShot(50, [this, id] {
        emit queryFlightTaskById(FlightTask::Instance().queryRecordById(id));
        });
}

void
WebSocketServer::queryFlightTaskByAircraftEntityIdReq(QString fjstid)
{
    QTimer::singleShot(50, [this, fjstid] {
        emit queryFlightTaskByAircraftEntityId(FlightTask::Instance().queryByAircraftEntityId(fjstid));
        });
}

void
WebSocketServer::queryFlightTaskByTimeRangeReq(QString startTime, QString endTime)
{
    QTimer::singleShot(50, [this, startTime, endTime] {
        emit queryFlightTaskByTimeRange(FlightTask::Instance().queryByTimeRange(startTime, endTime));
        });
}

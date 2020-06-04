#ifndef QFNETWORKREPLY_H
#define QFNETWORKREPLY_H

#include <qgsnetworkaccessmanager.h>

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRandomGenerator>


class QRandomGenerator;


class QfNetworkReply : public QObject
{

  Q_OBJECT

public:


  /**
   * A wrapper around QNetworkReply that allows retryable requests.
   * @param op HTTP method
   * @param nam network access manager
   * @param request the request to be performed
   * @param payload the request payload
   */
  QfNetworkReply( const QNetworkAccessManager::Operation operation, const QNetworkRequest request, const QByteArray payloadByteArray );


  /**
   * A wrapper around QNetworkReply that allows retryable requests.
   * @param op HTTP method
   * @param nam network access manager
   * @param request the request to be performed
   * @param payload the request payload
   */
  QfNetworkReply( const QNetworkAccessManager::Operation operation, const QNetworkRequest request, QHttpMultiPart *payloadMultiPart );


  /**
   * Aborts the current request and any other retries. Makes the current object into a final state.
   */
  void abort();


  /**
   * Get the QNetworkReply object once the CloudReply is finilized.
   * @return network reply
   */
  QNetworkReply *reply() const;


  /**
   * Reimplements QNetworkReply::ignoreSslErrors.
   * @param error a list of error to be ignored.
   */
  void ignoreSslErrors( QList<QSslError> errors );


  /**
   * Whether the request reached a final status.
   * @return true if the request reached a final status.
   */
  bool isFinished() const;


signals:

  /**
   * Replicates `QNetworkReply::downloadProgress` signal.
   * @note Because download may fail mid request and then retried, the bytesReceived may be reset back to 0.
   * @param bytesSent
   * @param bytesTotal
   */
  void downloadProgress( int bytesReceived, int bytesTotal );


  /**
   * Replicates `QNetworkReply::uploadProgress` signal.
   * @note Because upload may fail mid request and then retried, the bytesSent may be reset back to 0.
   * @param bytesSent
   * @param bytesTotal
   */
  void uploadProgress( int bytesSent, int bytesTotal );


  /**
   * Replicates `QNetworkReply::encrypted` signal.
   * @note May be called multiple times for each retry.
   */
  void encrypted();


  /**
   * Replicates `QNetworkReply::finished` signal. It is called only once, when the request was successfull, got a final error or ran out of retries.
   */
  void finished();


  // /////////////////
  // more than QNetworkReply signals
  // /////////////////
  /**
   * Emitted when a new retry is initiated.
   */
  void retry();


  /**
   * Emitted when a new error has occured.
   * @param code
   */
  void errorOccurred( QNetworkReply::NetworkError code );


  /**
   * Emitted when a new temporary error has occured. This is basically emitting the error that has occured during a retry.
   * @param code
   */
  void temporaryErrorOccurred( QNetworkReply::NetworkError code );

private:

  /**
   * The current HTTP method.
   */
  QNetworkAccessManager::Operation mOperation;


  /**
   * Whether the cloud reply has reached a final state.
   */
  bool mIsFinished = false;


  /**
   * Whether it is a multi-part request
   */
  bool mIsMultiPartPayload = false;


  /**
   * Number of retries left. Once the value reaches zero, the status of the last reply is the final status.
   */
  int mRetriesLeft = 5;


  /**
   * Upper bound of the delay between retries in milliseconds.
   */
  int mMaxTimeoutBetweenRetriesMs = 2000;


  /**
   * Expected SSL errors to be ignored.
   */
  QList<QSslError> mExpectedSslErrors;


  /**
   * Random number generator instance. Used to create random delay bettween retries.
   */
  QRandomGenerator mRNG;


  /**
   * Network access manager.
   */
  QgsNetworkAccessManager *mNetworkAccessManager = QgsNetworkAccessManager::instance();


  /**
   * The current request.
   */
  QNetworkRequest mRequest;


  /**
   * Request payload
   */
  const QByteArray mPayloadByteArray;


  /**
   * Request payload as multipart
   */
  QHttpMultiPart *mPayloadMultiPart;


  /**
   * The current outgoing request. If the request fails and can be retried, the object is disposed and replaced with a new one.
   */
  QNetworkReply *mReply = nullptr;


  /**
   * Binds signal listeners to `QNetworkReply` object
   */
  void initiateRequest();


  /**
   * Reemits `QNetworkReply::downloadProgress` signal.
   * @note Because download may fail mid request and then retried, the bytesSent may be reset back to 0.
   * @param bytesReceived
   * @param bytesTotal
   */
  void onDownloadProgress( int bytesReceived, int bytesTotal );


  /**
   * Reemits `QNetworkReply::uploadProgress` signal.
   * @note Because upload may fail mid request and then retried, the bytesSent may be reset back to 0.
   * @param bytesSent
   * @param bytesTotal
   */
  void onUploadProgress( int bytesSent, int bytesTotal );


  /**
   * Reemits `QnetworkReply::encrypted` signal.
   */
  void onEncrypted();


  /**
   * Called when a request attempt is finished. If needed, make a retry.
   */
  void onFinished();
};


#endif // QFNETWORKREPLY_H
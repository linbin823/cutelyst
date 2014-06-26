/*
 * Copyright (C) 2013 Daniel Nicoletti <dantti12@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef CUTELYST_APPLICATION_H
#define CUTELYST_APPLICATION_H

#include <QCoreApplication>

namespace Cutelyst {

namespace Plugin {
class AbstractPlugin;
}

class Context;
class Controller;
class DispatchType;
class Request;
class Response;
class Engine;
class ApplicationPrivate;
class Application : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Application)
public:
    /**
     * @brief Application
     * @param parent
     *
     * This is the main class of a Cutelyst appplication, here
     * we configure settings, register controller classes,
     * plugins and dispatchers.
     *
     * A Web Engine will instantiate your application through this
     * class, next it will load the settings file, and in the end
     * it will call \sa init() which is where your application
     * should do it's setup.
     *
     * \warning DO NOT register your controllers,
     * plugins or anything that might want to
     * use \sa config(), do that in \sa initApplication()
     *
     */
    explicit Application(QObject *parent = 0);
    virtual ~Application();

    /**
     * @brief init
     *
     * Do your application initialization here, if your
     * application should not proceed log some information
     * that might help on debuggin and return false
     *
     * For example if your application only works with
     * PostgeSQL and the Qt driver is not available it
     * makes sense to fail here. However you should not
     * initialize resouces that cannot be shared among
     * process. \sa postFork
     *
     * @return true if your application successfuly initted
     */
    virtual bool init() = 0;

    /**
     * @brief postFork is called after the engine forks
     *
     * After the web engine forks itself it will call
     * this function so that you can initialize resources
     * that can't be shared with the parent process, namely
     * sockets and file descriptors.
     *
     * A good example of usage of this function is when
     * openning a connection to the database which can't
     * be shared with other process and should probably
     * make this function return false if it fails to open.
     *
     * Default implementation returns true.
     *
     * @return false if the engine should not use this process
     */
    virtual bool postFork();

    /**
     * Registers a global plugin ie one that doesn't need
     * to be created explicity for a single request and returns
     * true on plugin->isApplicationPlugin();
     *
     * @return true if the plugin could be registered
     */
    bool registerPlugin(Plugin::AbstractPlugin  *plugin);

    /**
     * @brief registerController
     *
     * This method register Controller classes which
     * are responsible for handlying the Requests,
     * since they are reused between multiple requests
     * beaware of not storing data there, instead you
     * might want to use a session plugin or the stash.
     *
     * @param controller class
     * @return true if succeeded
     */
    bool registerController(Controller *controller);

    /**
     * Register a custom DispatchType, if none is registered
     * all the built-in dispatchers types will be registered
     */
    void registerDispatcher(DispatchType *dispatcher);

    /**
     * @brief applicationName
     * @return default implementation returns the class name
     */
    virtual QByteArray applicationName() const;

    /**
     * @brief applicationVersion
     * @return default implementation returns QCoreApplication::applicationVersion()
     */
    virtual QByteArray applicationVersion() const;

Q_SIGNALS:
    /**
     * Emited so that you register all plugins that are specifically
     * to each request
     */
    void registerPlugins(Context *ctx);

protected:
    /**
     * @brief user configuration for the application
     * @return the configuration settings
     */
    QVariantHash config() const;

    friend class Engine;
    bool setup(Engine *engine);
    void handleRequest(Request *req);

    ApplicationPrivate *d_ptr;
};

}

#define CutelystApplicationInterface_iid "org.cutelyst.CutelystApplicationInterface"

Q_DECLARE_INTERFACE(Cutelyst::Application, CutelystApplicationInterface_iid)

#endif // CUTELYST_APPLICATION_H

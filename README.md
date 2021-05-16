**数据库连接池的思想**

初始化固定数目的连接，把这些连接存放在连接池中，当应用程序执行一个数据库事务，只需要从数据库连接池中取出一个空闲的数据库连接，当数据库连接池中没有空闲的连接时并且数据库连接池的连接数量没有达到最大的时候，需要创建一个新的数据库连接，然后将其放在数据连接池中，并将其取出给该应用程序使用。当应用程序使用完成之后需要归还该连接。

**需要注意的问题**

1. 限制连接池中最多可以容纳的连接数量，避免创建过多的连接消耗大量的资源
2. 如果数据库连接池的连接数量达到最大，如果客户来请求并且没有空闲的连接需要创建一个临时的连接，当客户使用结束后，需要将这个连接释放；如果数据库连接池中的数量没有达到最大时，可以将创建的新数据库连接放到数据库连接池中。
3. 当客户使用连接完成之后，需要此连接归还到数据连接池中
4. 需要考虑连接空闲时间，当数据库连接池中的空闲连接超过一定时间时，需要考虑该连接池最多保留几个连接，同时需要将其他连接都释放，这样才能节省资源。

**连接池的实现**

1. 定义数据库连接池的结构
2. 初始化数据库连接池
3. 取出连接
4. 释放连接
5. 关闭数据库连接池
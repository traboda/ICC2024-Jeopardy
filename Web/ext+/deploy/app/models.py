from database import db

class ExtensionUser(db.Model):
    __tablename__ = 'extension_users'
    id = db.Column(db.Integer, primary_key=True)
    user_id = db.Column(db.String(50), unique=True, nullable=False)
    password = db.Column(db.String(50), nullable=False)

    def __repr__(self):
        return f"<ExtensionUser {self.user_id}>"

class FrontendUser(db.Model):
    __tablename__ = 'frontend_users'
    id = db.Column(db.Integer, primary_key=True)
    email = db.Column(db.String(100), unique=True, nullable=False)
    password = db.Column(db.String(50), nullable=False)
    
    def __repr__(self):
        return f"<FrontendUser {self.email}>"

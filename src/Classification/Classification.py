#!/usr/bin/env python
# coding: utf-8

# # Classification

# ### Imports

# In[ ]:


import os
#Disable GPU
os.environ['CUDA_VISIBLE_DEVICES'] = ''

import matplotlib.pyplot as plt
import numpy as np
import random
import tensorflow as tf
import sys
import glob
from pathlib import Path
from tensorflow.keras import applications
from tensorflow.keras import layers
from tensorflow.keras import losses
from tensorflow.keras import optimizers
from tensorflow.keras import metrics
from tensorflow.keras import Model
from tensorflow.keras.applications import inception_v3


target_shape = (250, 250)


# #### Importing dataset

# In[ ]:


def preprocess_image(filename):
    """
    Load the specified file as a JPEG image, preprocess it and
    resize it to the target shape.
    """

    image_string = tf.io.read_file(filename)
    image = tf.image.decode_jpeg(image_string, channels=3)
    image = tf.image.resize(image, target_shape)
    return image


def preprocess_triplets(anchor, positive, negative):
    """
    Given the filenames corresponding to the three images, load and
    preprocess them.
    """

    return (
        preprocess_image(anchor),
        preprocess_image(positive),
        preprocess_image(negative),
    )


# In[ ]:


path_root = "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
path_root = Path("../../build/bin" if path_root[0] == '$' else path_root).resolve()
print(path_root)


# In[ ]:


def loadDataset(folder):
    anchor_images = []
    positive_images = []
    negative_images = []
    
    filelist = sorted(filter(os.path.isfile, glob.glob(str(folder / '*'))))
    for i in range(0, len(filelist), 3):
        anchor_images.append(filelist[i])
        positive_images.append(filelist[i+1])
        negative_images.append(filelist[i+2])

    anchor_dataset = tf.data.Dataset.from_tensor_slices(anchor_images)
    positive_dataset = tf.data.Dataset.from_tensor_slices(positive_images)
    negative_dataset = tf.data.Dataset.from_tensor_slices(negative_images)

    dataset = tf.data.Dataset.zip((anchor_dataset, positive_dataset, negative_dataset))
    dataset = dataset.shuffle(buffer_size=1024)
    dataset = dataset.map(preprocess_triplets)
    return dataset

# Let's now split our dataset in train and validation.
train_dataset = loadDataset(path_root / "triplets_large")
val_dataset = loadDataset(path_root / "triplets_large_test")

train_dataset = train_dataset.batch(32, drop_remainder=False)
train_dataset = train_dataset.prefetch(16)

val_dataset = val_dataset.batch(32, drop_remainder=False)
val_dataset = val_dataset.prefetch(16)


# ## Setting up the model

# ### Generator model

# In[ ]:


base_cnn = inception_v3.InceptionV3(
    include_top=False,
    weights="imagenet",
    input_shape=target_shape + (3,)
)

flatten = layers.Flatten()(base_cnn.output)

dense1 = layers.Dense(256, activation="relu")(flatten)
dense1 = layers.BatchNormalization()(dense1)
dropout1 = layers.Dropout(0.3)(dense1)

dense2 = layers.Dense(128, activation="relu")(dropout1)
dense2 = layers.BatchNormalization()(dense2)
dropout2 = layers.Dropout(0.3)(dense2)

output = layers.Dense(64)(dropout2)

embedding = Model(base_cnn.input, output, name="Embedding")

for layer in base_cnn.layers:
    layer.trainable = False


# ### Siamese network

# In[ ]:


class DistanceLayer(layers.Layer):
    """
    This layer is responsible for computing the distance between the anchor
    embedding and the positive embedding, and the anchor embedding and the
    negative embedding.
    """

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def call(self, anchor, positive, negative):
        ap_distance = tf.reduce_sum(tf.square(anchor - positive), -1)
        an_distance = tf.reduce_sum(tf.square(anchor - negative), -1)
        return (ap_distance, an_distance)


anchor_input = layers.Input(name="anchor", shape=target_shape + (3,))
positive_input = layers.Input(name="positive", shape=target_shape + (3,))
negative_input = layers.Input(name="negative", shape=target_shape + (3,))

distances = DistanceLayer()(
    embedding(inception_v3.preprocess_input(anchor_input)),
    embedding(inception_v3.preprocess_input(positive_input)),
    embedding(inception_v3.preprocess_input(negative_input)),
)

siamese_network = Model(
    inputs=[anchor_input, positive_input, negative_input], outputs=distances
)


# In[ ]:


class SiameseModel(Model):
    """The Siamese Network model with a custom training and testing loops.

    Computes the triplet loss using the three embeddings produced by the
    Siamese Network.

    The triplet loss is defined as:
       L(A, P, N) = max(‖f(A) - f(P)‖² - ‖f(A) - f(N)‖² + margin, 0)
    """

    def __init__(self, siamese_network, margin=0.5):
        super(SiameseModel, self).__init__()
        self.siamese_network = siamese_network
        self.margin = margin
        self.loss_tracker = metrics.Mean(name="loss")

    def call(self, inputs):
        return self.siamese_network(inputs)

    def train_step(self, data):
        # GradientTape is a context manager that records every operation that
        # you do inside. We are using it here to compute the loss so we can get
        # the gradients and apply them using the optimizer specified in
        # `compile()`.
        with tf.GradientTape() as tape:
            loss = self._compute_loss(data)

        # Storing the gradients of the loss function with respect to the
        # weights/parameters.
        gradients = tape.gradient(loss, self.siamese_network.trainable_weights)

        # Applying the gradients on the model using the specified optimizer
        self.optimizer.apply_gradients(
            zip(gradients, self.siamese_network.trainable_weights)
        )

        # Let's update and return the training loss metric.
        self.loss_tracker.update_state(loss)
        return {"loss": self.loss_tracker.result()}

    def test_step(self, data):
        loss = self._compute_loss(data)

        # Let's update and return the loss metric.
        self.loss_tracker.update_state(loss)
        return {"loss": self.loss_tracker.result()}

    def _compute_loss(self, data):
        # The output of the network is a tuple containing the distances
        # between the anchor and the positive example, and the anchor and
        # the negative example.
        ap_distance, an_distance = self.siamese_network(data)

        # Computing the Triplet Loss by subtracting both distances and
        # making sure we don't get a negative value.
        loss = ap_distance - an_distance
        loss = tf.maximum(loss + self.margin, 0.0)
        return loss

    @property
    def metrics(self):
        # We need to list our metrics here so the `reset_states()` can be
        # called automatically.
        return [self.loss_tracker]


# In[ ]:


siamese_model = SiameseModel(siamese_network)
siamese_model.compile(optimizer=optimizers.Adam(learning_rate=0.000001), weighted_metrics=[])
siamese_model.fit(train_dataset, epochs=10, validation_data=val_dataset)


# In[ ]:


siamese_model.metrics_names
plt.plot(siamese_model.history.history['loss'])
plt.plot(siamese_model.history.history['val_loss'])
plt.title('model_loss')
plt.xlabel('epoch')
plt.ylabel('loss')
plt.ylim(0, 1)
plt.legend(['loss', 'val_loss'], loc='upper right')

plt.show()


# In[ ]:


sample = next(iter(val_dataset))

anchor, positive, negative = sample
anchor_embedding, positive_embedding, negative_embedding = (
    embedding(inception_v3.preprocess_input(anchor)),
    embedding(inception_v3.preprocess_input(positive)),
    embedding(inception_v3.preprocess_input(negative)),
)


# In[ ]:


cosine_similarity = metrics.CosineSimilarity()

positive_similarity = cosine_similarity(anchor_embedding, positive_embedding)
print("Positive similarity:", positive_similarity.numpy())
negative_similarity = cosine_similarity(anchor_embedding, negative_embedding)
print("Negative similarity:", negative_similarity.numpy())


mean_pos = metrics.Mean()
mean_neg = metrics.Mean()

for anchor, positive, negative in val_dataset:
    anchor_embedding, positive_embedding, negative_embedding = (
        embedding(inception_v3.preprocess_input(anchor)),
        embedding(inception_v3.preprocess_input(positive)),
        embedding(inception_v3.preprocess_input(negative)),
    )
    mean_pos.update_state(tf.reduce_sum(tf.square(anchor_embedding - positive_embedding), -1))
    mean_neg.update_state(tf.reduce_sum(tf.square(anchor_embedding - negative_embedding), -1))
print("Positive distance:", mean_pos.result().numpy())
print("Negative distance:", mean_neg.result().numpy())


# ### Saving the model

# In[ ]:


embedding.save("model_trained.h5", include_optimizer=False)


# In[ ]:


embedding = tf.keras.models.load_model("model_trained.h5")


# In[ ]:




